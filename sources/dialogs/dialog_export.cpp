/***************************************************************************
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

#include "dialog_export.h"
#include "ui_dialog_export.h"
#include "soundfontmanager.h"
#include "contextmanager.h"
#include <QFileDialog>
#include <QMessageBox>

DialogExport::DialogExport(SoundfontManager *sf2, QList<EltID> listSf2, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogExport)
{
    // Preparation of the interface
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint));

    // Populate the list
    foreach (EltID id, listSf2)
    {
        id.typeElement = elementSf2;
        QTreeWidgetItem * rootItem = new QTreeWidgetItem(QStringList(sf2->getQstr(id, champ_name)));
        ui->listPresets->addTopLevelItem(rootItem);

        id.typeElement = elementPrst;
        foreach (int i, sf2->getSiblings(id))
        {
            id.indexElt = i;
            QTreeWidgetItem * item = new QTreeWidgetItem(
                        QStringList() << QString("%1:%2   %3").arg(QString::number(sf2->get(id, champ_wBank).wValue), 3, QChar('0'))
                        .arg(QString::number(sf2->get(id, champ_wPreset).wValue), 3, QChar('0'))
                        .arg(sf2->getQstr(id, champ_name)));
            item->setCheckState(0, Qt::Checked);
            item->setData(0, Qt::UserRole, QPoint(id.indexSf2, id.indexElt));
            rootItem->addChild(item);
            rootItem->setExpanded(true);
        }

        // Tri
        rootItem->sortChildren(0, Qt::AscendingOrder);
    }
    ui->listPresets->resizeColumnToContents(0);

    int exportType = ContextManager::configuration()->getValue(ConfManager::SECTION_EXPORT, "type", 0).toInt();
    if (exportType < 0 || exportType >= ui->comboFormat->count())
        exportType = 0;
    ui->comboFormat->setCurrentIndex(exportType);
    on_comboFormat_currentIndexChanged(exportType);
    ui->lineFolder->setText(ContextManager::recentFile()->getLastDirectory(RecentFileManager::FILE_TYPE_EXPORT));

    ui->checkBank->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_EXPORT, "bank_directory", false).toBool());
    ui->checkPreset->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_EXPORT, "preset_prefix", true).toBool());
    ui->checkGM->setChecked(ContextManager::configuration()->getValue(ConfManager::SECTION_EXPORT, "gm_sort", false).toBool());

    int exportQuality = ContextManager::configuration()->getValue(ConfManager::SECTION_EXPORT, "quality", 1).toInt();
    if (exportQuality < 0 || exportQuality >= ui->comboQuality->count())
        exportQuality = 1;
    ui->comboQuality->setCurrentIndex(2 - exportQuality);
}

DialogExport::~DialogExport()
{
    delete ui;
}

void DialogExport::on_pushTick_clicked()
{
    // Tout sélectionner
    int nbTopLevelItems = ui->listPresets->topLevelItemCount();
    for (int i = 0; i < nbTopLevelItems; i++)
    {
        QTreeWidgetItem * rootItem = ui->listPresets->topLevelItem(i);
        int nbItems = rootItem->childCount();
        for (int j = 0; j < nbItems; j++)
            rootItem->child(j)->setCheckState(0, Qt::Checked);
    }
}

void DialogExport::on_pushUntick_clicked()
{
    // Tout décocher
    int nbTopLevelItems = ui->listPresets->topLevelItemCount();
    for (int i = 0; i < nbTopLevelItems; i++)
    {
        QTreeWidgetItem * rootItem = ui->listPresets->topLevelItem(i);
        int nbItems = rootItem->childCount();
        for (int j = 0; j < nbItems; j++)
            rootItem->child(j)->setCheckState(0, Qt::Unchecked);
    }
}

void DialogExport::on_pushFolder_clicked()
{
    QString qDir = QFileDialog::getExistingDirectory(this, trUtf8("Choisir un répertoire de destination"),
                                                     ui->lineFolder->text());
    if (!qDir.isEmpty())
        ui->lineFolder->setText(qDir);
}

void DialogExport::on_pushAnnuler_clicked()
{
    QDialog::close();
}

void DialogExport::on_pushExport_clicked()
{
    // Sauvegarde des paramètres
    ContextManager::configuration()->setValue(ConfManager::SECTION_EXPORT, "typ", ui->comboFormat->currentIndex());
    if (!ui->lineFolder->text().isEmpty() && QDir(ui->lineFolder->text()).exists())
        ContextManager::recentFile()->addRecentFile(RecentFileManager::FILE_TYPE_EXPORT, ui->lineFolder->text() + "/soundfont.sfz");
    else
    {
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Le répertoire n'est pas valide."));
        return;
    }

    // Création liste des presets à exporter et comptage
    int numberOfSoundfonts = 0;
    int maxNumberOfPresets = 0;
    QList<QList<EltID> > listID;
    int nbRootItems = ui->listPresets->topLevelItemCount();
    for (int i = 0; i < nbRootItems; i++)
    {
        int numberOfPresets = 0;
        QList<EltID> subListID;
        QTreeWidgetItem * rootItem = ui->listPresets->topLevelItem(i);
        int nbItems = rootItem->childCount();
        for (int j = 0; j < nbItems; j++)
        {
            QTreeWidgetItem * item = rootItem->child(j);
            if (item->checkState(0) == Qt::Checked)
            {
                numberOfPresets++;
                QPoint dataPoint = item->data(0, Qt::UserRole).toPoint();
                subListID << EltID(elementPrst, dataPoint.x(), dataPoint.y(), 0, 0);
            }
        }

        if (numberOfPresets > 0)
        {
            listID << subListID;
            numberOfSoundfonts++;
            if (numberOfPresets > maxNumberOfPresets)
                maxNumberOfPresets = numberOfPresets;
        }
    }

    if (numberOfSoundfonts > 1)
    {
        if (maxNumberOfPresets > 127)
        {
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Dans le cas où plusieurs soundfonts sont exportées, "
                                                                   "le nombre maximal de presets par soundfont est de 127."));
            return;
        }
        else if (numberOfSoundfonts > 127)
        {
            QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Le nombre maximal de soundfonts à exporter est de 127."));
            return;
        }
    }

    if (maxNumberOfPresets == 0)
    {
        QMessageBox::warning(this, trUtf8("Attention"), trUtf8("Au moins un preset doit être sélectionné."));
        return;
    }

    ContextManager::configuration()->setValue(ConfManager::SECTION_EXPORT, "preset_prefix", ui->checkPreset->isChecked());
    ContextManager::configuration()->setValue(ConfManager::SECTION_EXPORT, "bank_directory", ui->checkBank->isChecked());
    ContextManager::configuration()->setValue(ConfManager::SECTION_EXPORT, "gm_sort", ui->checkGM->isChecked());
    ContextManager::configuration()->setValue(ConfManager::SECTION_EXPORT, "quality", 2 - ui->comboQuality->currentIndex());

    emit(accepted(listID, ui->lineFolder->text(), ui->comboFormat->currentIndex(),
                  ui->checkPreset->isChecked(), ui->checkBank->isChecked(),
                  ui->checkGM->isChecked(), 2 - ui->comboQuality->currentIndex()));
    QDialog::accept();
}

void DialogExport::on_comboFormat_currentIndexChanged(int index)
{
    // Options for sf3
    ui->labelQuality->setVisible(index == 1);
    ui->comboQuality->setVisible(index == 1);

    // Options for sfz
    ui->checkBank->setVisible(index == 2);
    ui->checkGM->setVisible(index == 2);
    ui->checkPreset->setVisible(index == 2);
}
