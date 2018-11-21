#include "toolchords.h"
#include "toolchords_gui.h"
#include "soundfontmanager.h"
#include "runnablechordcreator.h"

ToolChords::ToolChords() : AbstractTool(new ToolChords_parameters(), new ToolChords_gui()),
    _waitingDialog(nullptr)
{
    connect(this, SIGNAL(elementProcessed(EltID, int, int, double)), this, SLOT(onElementProcessed(EltID, int, int, double)), Qt::QueuedConnection);
}

ToolChords::~ToolChords()
{
    delete _waitingDialog;
}

bool ToolChords::isCompatible(IdList ids)
{
    // Ok if there is only one instrument
    return ids.getSelectedIds(elementInst).count() == 1;
}

void ToolChords::runInternal(SoundfontManager * sm, QWidget * parent, IdList ids, AbstractToolParameters * parameters)
{
    ToolChords_parameters * params = (ToolChords_parameters *)parameters;
    _canceled = false;
    _warning = "";
    bool loopEnabled = params->getLoopSample();

    // Current instrument
    EltID idInst = ids.getSelectedIds(elementInst)[0];
    idInst.typeElement = elementInst;
    if (sm->getSiblings(EltID(elementInstSmpl, idInst.indexSf2, idInst.indexElt)).empty())
    {
        _warning = trUtf8("L'instrument doit contenir des sons.");
        finished(true);
        return;
    }

    // Create a new instrument
    _instrumentName = params->getInstrumentName();
    _idNewInst = idInst;
    _idNewInst.indexElt = sm->add(_idNewInst);
    sm->set(_idNewInst, champ_name, _instrumentName.left(20));
    if (loopEnabled)
    {
        AttributeValue value;
        value.wValue = 1;
        sm->set(_idNewInst, champ_sampleModes, value);
    }

    // Number of keys per sample
    int keyNumber = 1;
    switch (params->getDensityType())
    {
    case 0: keyNumber = 1; break;
    case 1: keyNumber = 3; break;
    case 2: keyNumber = 6; break;
    default:
        return;
    }

    // Compute the number of steps
    _currentStep = 0;
    rangesType range = this->getInstrumentRange(idInst);
    _steps = qAbs(range.byHi - range.byLo) / keyNumber + 1;
    _stereoSamples = params->getStereoSample();
    if (_stereoSamples)
        _steps *= 2;

    // Create and open a progress dialog
    if (_waitingDialog != nullptr)
        delete _waitingDialog;
    _waitingDialog = new WaitingToolDialog(this->getLabel(), _steps, parent);
    _waitingDialog->show();
    connect(_waitingDialog, SIGNAL(canceled()), this, SLOT(onCancel()));

    // For each division
    int noteStart2 = range.byLo;
    int noteEnd = range.byHi;
    int noteStart = noteStart2 + (noteEnd - noteStart2) % keyNumber;

    // For each key
    ChordInfo ci = params->getChordConfiguration();
    for (int key = noteStart; key <= noteEnd; key += keyNumber)
    {
        // For each side (left / right)
        for (int side = 0; side < 1 + (_stereoSamples ? 1 : 0); side++)
        {
            RunnableChordCreator * rcc = new RunnableChordCreator(this, idInst, ci, key,
                                                                  qMax(noteStart2, key - keyNumber + 1), // min key
                                                                  loopEnabled, _stereoSamples, side);
            QThreadPool::globalInstance()->start(rcc);
        }
    }
}

void ToolChords::onElementProcessed(EltID idSmpl, int key, int minKey, double minAtt)
{
    if (_waitingDialog == nullptr)
        return; // Just in case
    SoundfontManager * sm = SoundfontManager::getInstance();

    // Sample name
    QString name;
    if (_stereoSamples)
        name = _instrumentName.left(15);
    else
        name = _instrumentName.left(16);
    QString str2 = QString("%1").arg(key, 3, 10, QChar('0'));
    bool isRight = false;
    if (_stereoSamples)
    {
        isRight = (sm->get(idSmpl, champ_sfSampleType).sfLinkValue == rightSample);
        if (isRight)
            name = name + ' ' + str2 + 'R';
        else
            name = name + ' ' + str2 + 'L';
    }
    else
        name = name + ' ' + str2;
    sm->set(idSmpl, champ_name, name);

    // Link samples
    AttributeValue value;
    if (_stereoSamples)
    {
        if (_sampleKey.contains(key))
        {
            EltID idLink = _sampleKey.take(key);
            value.wValue = idLink.indexElt;
            sm->set(idSmpl, champ_wSampleLink, value);
            value.wValue = idSmpl.indexElt;
            sm->set(idLink, champ_wSampleLink, value);
        }
        else
            _sampleKey[key] = idSmpl;
    }

    // Ajout du sample dans l'instrument
    EltID idInstSmpl = _idNewInst;
    idInstSmpl.typeElement = elementInstSmpl;
    idInstSmpl.indexElt2 = sm->add(idInstSmpl);

    // Configuration
    value.wValue = idSmpl.indexElt;
    sm->set(idInstSmpl, champ_sampleID, value);
    value.rValue.byLo = minKey;
    value.rValue.byHi = key;
    sm->set(idInstSmpl, champ_keyRange, value);
    if (_stereoSamples)
    {
        if (isRight)
            value.shValue = 500;
        else
            value.shValue = -500;
    }
    else
        value.shValue = 0;
    sm->set(idInstSmpl, champ_pan, value);
    value.wValue = minAtt * 10;
    sm->set(idInstSmpl, champ_initialAttenuation, value);

    // Update the waiting dialog
    _waitingDialog->setValue(++_currentStep);
    if (_currentStep >= _steps)
    {
        delete _waitingDialog;
        _waitingDialog = nullptr;
        if (_canceled)
        {
            SoundfontManager::getInstance()->revertNewEditing();
            emit(finished(false));
        }
        else
            emit(finished(true));
    }
}

void ToolChords::onCancel()
{
    _canceled = true;
    QThreadPool::globalInstance()->clear();
}

QString ToolChords::getWarning()
{
    return _warning;
}

rangesType ToolChords::getInstrumentRange(EltID idInst)
{
    rangesType result;
    result.byLo = 127;
    result.byHi = 0;

    // Check the range of all divisions
    SoundfontManager * sm = SoundfontManager::getInstance();
    EltID idInstSmpl(elementInstSmpl, idInst.indexSf2, idInst.indexElt);
    bool useDefault = false;
    foreach (int index, sm->getSiblings(idInstSmpl))
    {
        idInstSmpl.indexElt2 = index;

        if (sm->isSet(idInstSmpl, champ_keyRange))
        {
            rangesType rangeTmp = sm->get(idInstSmpl, champ_keyRange).rValue;
            if (rangeTmp.byLo < result.byLo)
                result.byLo = rangeTmp.byLo;
            if (rangeTmp.byHi > result.byHi)
                result.byHi = rangeTmp.byHi;
        }
        else
            useDefault = true;
    }

    // Check the range of the instrument, if needed
    if (useDefault && sm->isSet(idInst, champ_keyRange))
    {
        rangesType defaultRange = sm->get(idInst, champ_keyRange).rValue;
        if (defaultRange.byLo < result.byLo)
            result.byLo = defaultRange.byLo;
        if (defaultRange.byHi > result.byHi)
            result.byHi = defaultRange.byHi;
    }

    // Final check
    if (result.byHi < result.byLo)
    {
        quint8 tmp = result.byLo;
        result.byLo = result.byHi;
        result.byHi = tmp;
    }

    return result;
}
