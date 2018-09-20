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

#ifndef TREE_H
#define TREE_H

#include "qtreewidget.h"
#include "sf2_types.h"

class SoundfontManager;
class MainWindowOld;
class QComboBox;
class TreeWidgetItem;

class Tree : public QTreeWidget
{
    Q_OBJECT
public:
    class menuClicDroit
    {
    public :
        menuClicDroit(MainWindowOld *_mainWindow);
        ~menuClicDroit();

        QAction *nouveauSample;
        QAction *nouvelInstrument;
        QAction *nouveauPreset;
        QAction *remplacer;
        QAction *associer;
        QAction *copier;
        QAction *coller;
        QAction *supprimer;
        QAction *renommer;
        QAction *fermer;
        QMenu *menu;
    };
    explicit Tree(QWidget *parent = 0);
    ~Tree();

    void init(MainWindowOld * mainWindow, SoundfontManager *sf2, QComboBox * comboSf2);
    unsigned int getSelectedItemsNumber();
    bool isSelectedItemsTypeUnique();
    bool isSelectedItemsSf2Unique();
    bool isSelectedItemsFamilyUnique();
    EltID getFirstID();
    QList<EltID> getAllIDs();
    EltID getNextID(bool closeFile);
    void selectNone(bool _refresh = false);
    void select(EltID id, bool _refresh = false);
    void select(QList<EltID> ids);
    void desactiveSuppression();
    void activeSuppression();
    void clearPastedID();
    EltID getElementToSelectAfterDeletion();

public slots:
    void collapse() {this->trier(1);}       // Clic sur "enrouler"
    void searchTree(QString qStr);          // Lors d'une modification du champ recherche
    void clicTree();                        // Modification de la sélection dans l'arborescence
    void clicTreeRight();                   // Clic droit dans l'arborescence
    void comboSf2IndexChanged(int index);

    // Update by the sf2 core
    void newElement(EltID id);
    void hideElement(EltID id, bool isHidden);
    void changeElementName(EltID id, QString name);
    void changeElementOrder(EltID id, QString order, bool sort);
    void removeElement(EltID id);

signals:
    void dropped(EltID dest, EltID src, int temps, int *msg, QByteArray *ba1, QByteArray *ba2);

protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent * event);
    virtual void dropEvent(QDropEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

    QStringList mimeTypes() const
    {
        QStringList qstrList;
        qstrList.append("text/uri-list");
        return qstrList;
    }

private:
    // Attributs privés
    MainWindowOld *_mainWindow;
    menuClicDroit *_treeMenu;
    bool _refresh;
    bool _updateSelectionInfo;
    QList<EltID> _idList;
    unsigned int _infoSelectedItemsNumber;
    bool _infoIsSelectedItemsTypeUnique;
    bool _infoIsSelectedItemsSf2Unique;
    bool _infoIsSelectedItemsFamilyUnique;
    SoundfontManager * _sf2;
    QComboBox * _comboSf2;
    QList<EltID> _displayedElements;
    QString _searchedText;
    int _displayedSf2;

    // Méthodes privées
    void updateSelectionInfo();
    TreeWidgetItem *selectedItem(unsigned int pos);
    void supprimerElt();
    void displaySample(int idSf2, int index, bool repercute = true);
    void displayInstrument(int idSf2, int index, bool repercuteSmpl = true, bool repercutePrst = true);
    void displayPreset(int idSf2, int index, bool repercute = true);
    void displayElement(EltID id);
    EltID getNextSample();
    EltID getNextInst();
    EltID getNextInstSmpl(int numInst);
    EltID getNextPrst();
    EltID getNextPrstInst(int numPrst);
    void searchTree(QString qStr, int displayedSf2);
    QList<TreeWidgetItem *> getEltsToId(EltID id, bool includeHidden = false);
    void renameSf2InComboBox(int numSf2, QString name);
    void addSf2InComboBox(int numSf2);
    void removeSf2FromComboBox(int numSf2);
    void trier(bool reinitCollapse);
};

#endif // TREE_H
