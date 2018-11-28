#include "toolsampleexport.h"
#include "soundfontmanager.h"
#include <qmath.h>
#include <QFileDialog>
#include "contextmanager.h"
#include <QApplication>

void ToolSampleExport::beforeProcess(IdList ids)
{
    Q_UNUSED(ids)
    _exportedSamples.clear();

    // Directory in which the samples will be exported
    _dirPath = QFileDialog::getExistingDirectory(
                QApplication::activeWindow(), trUtf8("Choose a destination folder"),
                ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_SAMPLE));
    if (!_dirPath.isEmpty())
    {
        _dirPath += "/";
        ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_SAMPLE, _dirPath + "sample.wav");
    }
}

void ToolSampleExport::process(SoundfontManager * sm, EltID id, AbstractToolParameters *parameters)
{
    Q_UNUSED(parameters);

    // Path specified?
    if (_dirPath.isEmpty())
        return;

    // Sample already processed?
    bool isStereo = false;
    EltID id2 = id;
    _mutex.lock();
    if (_exportedSamples.contains(id))
    {
        _mutex.unlock();
        return;
    }
    _exportedSamples << id;

    // Stereo sample?
    if (sm->get(id, champ_sfSampleType).wValue != monoSample &&
            sm->get(id, champ_sfSampleType).wValue != RomMonoSample)
    {
        id2.indexElt = sm->get(id, champ_wSampleLink).wValue;
        _exportedSamples << id2;
        isStereo = true;
    }

    // Filename of the exported sample (in the mutex also so that filenames are all different)
    QString fileName = getFilePath(sm, id, id2, isStereo);
    _mutex.unlock();

    // Export
    if (isStereo)
    {
        // First id must be the left sound
        if (sm->get(id, champ_sfSampleType).wValue == rightSample || sm->get(id, champ_sfSampleType).wValue == RomRightSample)
        {
            EltID idTmp = id;
            id = id2;
            id2 = idTmp;
        }

        Sound::exporter(fileName, sm->getSound(id), sm->getSound(id2));
    }
    else
        Sound::exporter(fileName, sm->getSound(id));
}

QString ToolSampleExport::getFilePath(SoundfontManager * sm, EltID id1, EltID id2, bool isStereo)
{
    QString fileName;
    if (isStereo)
    {
        QString fileName1 = sm->getQstr(id1, champ_name);
        QString fileName2 = sm->getQstr(id2, champ_name);
        int nb = Sound::lastLettersToRemove(fileName1, fileName2);
        fileName = fileName1.left(fileName1.size() - nb).replace(QRegExp("[:<>\"/\\\\\\*\\?\\|]"), "_");
    }
    else
        fileName = sm->getQstr(id1, champ_name).replace(QRegExp("[:<>\"/\\\\\\*\\?\\|]"), "_");

    QString filePath = _dirPath + fileName;

    // Already existing?
    if (QFile::exists(filePath + ".wav"))
    {
        // Add a suffix
        int indice = 1;
        while (QFile::exists(filePath + "-" + QString::number(indice) + ".wav"))
            indice++;
        filePath += "-" + QString::number(indice);
    }

    return filePath + ".wav";
}
