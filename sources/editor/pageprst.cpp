﻿/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "pageprst.h"
#include "ui_pageprst.h"
#include "editor_old.h"
#include "thememanager.h"
#include <QMenu>

// Constructeur, destructeur
PagePrst::PagePrst(QWidget *parent) :
    PageTable(PAGE_PRST, parent),
    ui(new Ui::PagePrst)
{
    ui->setupUi(this);

    // Style
    ui->frameBottom->setStyleSheet("QFrame{background-color:" +
                                   ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name() + ";color:" +
                                   ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name() + "}");
    ui->tablePrst->setStyleSheet("QTableWidget{border:1px solid " +
                                 this->palette().dark().color().name() +
                                 ";border-top:0;border-left:0;border-right:0}");
    ui->pushCopyMod->setIcon(ContextManager::theme()->getColoredSvg(":/icons/copy.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushPasteMod->setIcon(ContextManager::theme()->getColoredSvg(":/icons/paste.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushNouveauMod->setIcon(ContextManager::theme()->getColoredSvg(":/icons/document-new.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));
    ui->pushSupprimerMod->setIcon(ContextManager::theme()->getColoredSvg(":/icons/minus.svg", QSize(16, 16), ThemeManager::BUTTON_TEXT));

    this->contenant = elementPrst;
    this->contenantGen = elementPrstGen;
    this->contenantMod = elementPrstMod;
    this->contenu = elementInst;
    this->lien = elementPrstInst;
    this->lienGen = elementPrstInstGen;
    this->lienMod = elementPrstInstMod;
    this->_table = ui->tablePrst;
    this->tableMod = ui->tableMod;
    this->spinAmount = ui->spinSource2;
    this->checkAbs = ui->checkAbs;
    this->pushNouveauMod = ui->pushNouveauMod;
    this->pushSupprimerMod = ui->pushSupprimerMod;
    this->comboSource1Courbe = ui->comboCourbeSource1;
    this->comboSource2Courbe = ui->comboCourbeSource2;
    this->comboSource1 = ui->comboSource1;
    this->comboSource2 = ui->comboSource2;
    this->comboDestination = ui->comboDestination;
    _pushCopyMod = ui->pushCopyMod;
    _rangeEditor = ui->rangeEditor;
    _envelopEditor = NULL;

    // Remplissage de comboDestination
    for (int i = 0; i < 35; i++)
        this->comboDestination->addItem(getGenName(this->getDestNumber(i)));
    this->comboDestination->setLimite(35);

    // Remplissage des combosources
    this->remplirComboSource(this->comboSource1);
    this->remplirComboSource(this->comboSource2);

    // Initialisation spinBoxes
    ui->spinBank->init(this);
    ui->spinPreset->init(this);

    // Initialisation menu de copie de modulateurs
    _menu = new QMenu();
    _menu->addAction("", this, SLOT(duplicateMod()));
    _menu->addAction("", this, SLOT(copyMod()));

    // Initialisation édition étendues
    ui->rangeEditor->init(_sf2);

#ifdef Q_OS_MAC
    this->table->setStyleSheet("QHeaderView::section:horizontal{padding: 4px 10px 4px 10px;}");
    QFont font = this->table->font();
    font.setPixelSize(10);
    this->table->setFont(font);
    ui->horizontalLayout_2->setSpacing(15);
#endif
    ui->tablePrst->verticalHeader()->setDefaultSectionSize(QFontMetrics(ui->tablePrst->font()).height() + 8);

    QFont font = ui->tablePrst->font();
    font.setPixelSize(11);
    ui->tablePrst->horizontalHeader()->setFont(font);
    ui->tablePrst->verticalHeader()->setFont(font);

    connect(this->_table, SIGNAL(actionBegin()), this, SLOT(actionBegin()));
    connect(this->_table, SIGNAL(actionFinished()), this, SLOT(actionFinished()));
    connect(this->_table, SIGNAL(openElement(EltID)), this, SLOT(onOpenElement(EltID)));
    connect(ui->rangeEditor, SIGNAL(updateKeyboard()), this, SLOT(customizeKeyboard()));
    connect(ui->rangeEditor, SIGNAL(keyTriggered(int,int)), this, SIGNAL(triggerNote(int, int)));
    connect(ui->rangeEditor, SIGNAL(divisionsSelected(IdList)), this, SIGNAL(selectedIdsChanged(IdList)));
}

PagePrst::~PagePrst()
{
    delete ui;
}

QList<Page::DisplayOption> PagePrst::getDisplayOptions(IdList selectedIds)
{
    return QList<DisplayOption>()
            << DisplayOption(1, ":/icons/table.svg", trUtf8("Table"))
            << DisplayOption(2, ":/icons/range.svg", trUtf8("Étendues"), selectedIds.isElementUnique(elementPrst));
}

void PagePrst::setModVisible(bool visible)
{
    ui->frameModulator->setVisible(visible);
}

bool PagePrst::updateInterface(QString editingSource, IdList selectedIds, int displayOption)
{
    Q_UNUSED(editingSource)

    if (selectedIds.empty())
        return false;

    _currentParentIds = selectedIds.getSelectedIds(elementInst);
    _currentIds = selectedIds;
    customizeKeyboard();

    if (_currentParentIds.count() == 1)
    {
        ui->frameBottom->setEnabled(true);
        ui->frameModulator->setEnabled(true);
        EltID id = _currentParentIds.first();
        id.typeElement = elementPrst;
        ui->spinBank->setValue(_sf2->get(id, champ_wBank).wValue);
        ui->spinPreset->setValue(_sf2->get(id, champ_wPreset).wValue);
        ui->labelPercussion->setVisible(_sf2->get(id, champ_wBank).wValue == 128);
    }
    else
    {
        ui->frameBottom->setEnabled(false);
        ui->frameModulator->setEnabled(false);
    }

    switch (displayOption)
    {
    case 1:
        this->afficheTable();
        ui->stackedWidget->setCurrentIndex(0);
        break;
    case 2:
        this->afficheRanges();
        ui->stackedWidget->setCurrentIndex(1);
        break;
    default: return false;
    }

    return true;
}

// TableWidgetPrst
TableWidgetPrst::TableWidgetPrst(QWidget *parent) : TableWidget(parent) {}

TableWidgetPrst::~TableWidgetPrst() {}

int TableWidgetPrst::getRow(quint16 champ)
{
    int row = -1;
    switch (champ)
    {
    case champ_keyRange: row = 0; break;
    case champ_velRange: row = 1; break;
    case champ_initialAttenuation: row = 2; break;
    case champ_pan: row = 3; break;
    case champ_coarseTune: row = 4; break;
    case champ_fineTune: row = 5; break;
    case champ_scaleTuning: row = 6; break;
    case champ_initialFilterFc: row = 7; break;
    case champ_initialFilterQ: row = 8; break;
    case champ_delayVolEnv: row = 9; break;
    case champ_attackVolEnv: row = 10; break;
    case champ_holdVolEnv: row = 11; break;
    case champ_decayVolEnv: row = 12; break;
    case champ_sustainVolEnv: row = 13; break;
    case champ_releaseVolEnv: row = 14; break;
    case champ_keynumToVolEnvHold: row = 15; break;
    case champ_keynumToVolEnvDecay: row = 16; break;
    case champ_delayModEnv: row = 17; break;
    case champ_attackModEnv: row = 18; break;
    case champ_holdModEnv: row = 19; break;
    case champ_decayModEnv: row = 20; break;
    case champ_sustainModEnv: row = 21; break;
    case champ_releaseModEnv: row = 22; break;
    case champ_modEnvToPitch: row = 23; break;
    case champ_modEnvToFilterFc: row = 24; break;
    case champ_keynumToModEnvHold: row = 25; break;
    case champ_keynumToModEnvDecay: row = 26; break;
    case champ_delayModLFO: row = 27; break;
    case champ_freqModLFO: row = 28; break;
    case champ_modLfoToPitch: row = 29; break;
    case champ_modLfoToFilterFc: row = 30; break;
    case champ_modLfoToVolume: row = 31; break;
    case champ_delayVibLFO: row = 32; break;
    case champ_freqVibLFO: row = 33; break;
    case champ_vibLfoToPitch: row = 34; break;
    case champ_chorusEffectsSend: row = 35; break;
    case champ_reverbEffectsSend: row = 36; break;
    }
    return row + 1;
}
Champ TableWidgetPrst::getChamp(int row)
{
    Champ champ = champ_unknown;
    switch (row - 1)
    {
    case 0: champ = champ_keyRange; break;
    case 1: champ = champ_velRange; break;
    case 2: champ = champ_initialAttenuation; break;
    case 3: champ = champ_pan; break;
    case 4: champ = champ_coarseTune; break;
    case 5: champ = champ_fineTune; break;
    case 6: champ = champ_scaleTuning; break;
    case 7: champ = champ_initialFilterFc; break;
    case 8: champ = champ_initialFilterQ; break;
    case 9: champ = champ_delayVolEnv; break;
    case 10: champ = champ_attackVolEnv; break;
    case 11: champ = champ_holdVolEnv; break;
    case 12: champ = champ_decayVolEnv; break;
    case 13: champ = champ_sustainVolEnv; break;
    case 14: champ = champ_releaseVolEnv; break;
    case 15: champ = champ_keynumToVolEnvHold; break;
    case 16: champ = champ_keynumToVolEnvDecay; break;
    case 17: champ = champ_delayModEnv; break;
    case 18: champ = champ_attackModEnv; break;
    case 19: champ = champ_holdModEnv; break;
    case 20: champ = champ_decayModEnv; break;
    case 21: champ = champ_sustainModEnv; break;
    case 22: champ = champ_releaseModEnv; break;
    case 23: champ = champ_modEnvToPitch; break;
    case 24: champ = champ_modEnvToFilterFc; break;
    case 25: champ = champ_keynumToModEnvHold; break;
    case 26: champ = champ_keynumToModEnvDecay; break;
    case 27: champ = champ_delayModLFO; break;
    case 28: champ = champ_freqModLFO; break;
    case 29: champ = champ_modLfoToPitch; break;
    case 30: champ = champ_modLfoToFilterFc; break;
    case 31: champ = champ_modLfoToVolume; break;
    case 32: champ = champ_delayVibLFO; break;
    case 33: champ = champ_freqVibLFO; break;
    case 34: champ = champ_vibLfoToPitch; break;
    case 35: champ = champ_chorusEffectsSend; break;
    case 36: champ = champ_reverbEffectsSend; break;
    default: champ = champ_unknown;
    }
    return champ;
}

void PagePrst::spinUpDown(int steps, SpinBox *spin)
{
    EltID id = _currentIds[0];
    id.typeElement = elementPrst;
    int increment;
    if (steps > 0)
        increment = 1;
    else if (steps < 0)
        increment = -1;
    else return;
    int nbIncrement = 1;
    bool valPossible, bTmp;
    int wPreset = ui->spinPreset->value();
    int wBank = ui->spinBank->value();
    int valInit = spin->value();
    do
    {
        if (spin == ui->spinBank)
        {
            // le numéro de banque est modifié, numéro de preset reste fixe
            wBank = valInit + nbIncrement * increment;
            bTmp = (_sf2->get(id, champ_wBank).wValue == wBank);
        }
        else
        {
            // le numéro de preset est modifié, numéro de banque reste fixe
            wPreset = valInit + nbIncrement * increment;
            bTmp = (_sf2->get(id, champ_wPreset).wValue == wPreset);
        }
        valPossible = _sf2->isAvailable(id, wBank, wPreset) || bTmp;
        nbIncrement++;
    } while (!valPossible && valInit + nbIncrement * increment < 128 \
             && valInit + nbIncrement * increment > -1);
    valInit += (nbIncrement-1) * increment;

    // modification de la valeur
    if (valPossible) spin->setValue(valInit);
}

void PagePrst::setBank()
{
    if (_preparingPage)
        return;
    _preparingPage = true;

    // Compare with the previous value
    EltID id = _currentIds[0];
    id.typeElement = elementPrst;
    int initVal = ui->spinBank->value();
    if (_sf2->get(id, champ_wBank).wValue != initVal)
    {
        // Possible value?
        int nBank = _sf2->get(id, champ_wBank).wValue;
        int nPreset = _sf2->get(id, champ_wPreset).wValue;
        int delta = 0;
        int sens = 0;
        do
        {
            if (initVal + delta < 129)
            {
                if (initVal + delta == nBank)
                    sens = 2;
                else if (_sf2->isAvailable(id, initVal + delta, nPreset))
                    sens = 1;
            }
            if (initVal - delta > -1 && sens == 0)
            {
                if (initVal - delta == nBank)
                    sens = -2;
                else if (_sf2->isAvailable(id, initVal - delta, nPreset))
                    sens = -1;
            }
            delta++;
        } while (sens == 0 && delta < 129);
        if (sens == 1 || sens == -1)
        {
            initVal += sens * (delta-1);
            ui->spinBank->setValue(initVal);

            // Save the new value
            Valeur val;
            val.wValue = initVal;
            _sf2->set(id, champ_wBank, val);
            _sf2->endEditing(getEditingSource());
        }
        else
        {
            // Restore the previous value
            ui->spinBank->setValue(nBank);
        }
    }
    _preparingPage = 0;
    ui->labelPercussion->setVisible(ui->spinBank->value() == 128);
}

void PagePrst::setPreset()
{
    if (_preparingPage)
        return;
    _preparingPage = true;

    // Compare with the previous value
    EltID id = _currentIds[0];
    id.typeElement = elementPrst;
    int initVal = ui->spinPreset->value();
    if (_sf2->get(id, champ_wPreset).wValue != initVal)
    {
        // Possible value?
        initVal = _sf2->closestAvailablePreset(id, _sf2->get(id, champ_wBank).wValue, initVal);
        int nPreset = _sf2->get(id, champ_wPreset).wValue;
        if (initVal >= 0 && initVal != nPreset)
        {
            ui->spinPreset->setValue(initVal);

            // Save the new value
            Valeur val;
            val.wValue = initVal;
            _sf2->set(id, champ_wPreset, val);
            _sf2->endEditing(getEditingSource());
        }
        else
        {
            // Restore the previous value
            ui->spinPreset->setValue(nPreset);
        }
    }
    _preparingPage = 0;
}
