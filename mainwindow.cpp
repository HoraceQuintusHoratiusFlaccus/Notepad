#include <QtWidgets>

#include "mainwindow.h"

MainWindow::MainWindow()
    : TextEdit(new QPlainTextEdit)
{
    setCentralWidget(TextEdit);

    CreateActions();
    CreateStatusBar();

    ReadSettings();

    connect(TextEdit -> document(), &QTextDocument::contentsChanged,
            this, &MainWindow::DocumentWasModified);

#ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest,
            this, &MainWindow::CommitData);
#endif

    SetCurrentFile(QString());
}

void MainWindow::closeEvent(QCloseEvent * Event)
{
    if (MaybeSave())
    {
        WriteSettings();
        Event -> accept();
    }
    else
        Event -> ignore();
}

void MainWindow::NewFile()
{
    if (MaybeSave())
    {
        TextEdit -> clear();
        SetCurrentFile(QString());
    }
}

void MainWindow::Open()
{
    if (MaybeSave())
    {
        QString FileName = QFileDialog::getOpenFileName(this);
        if (!FileName.isEmpty())
            LoadFile(FileName);
    }
}

bool MainWindow::Save()
{
    if (CurFile.isEmpty())
        return SaveAs();
    else
        return SaveFile(CurFile);
}

bool MainWindow::SaveAs()
{
    QFileDialog Dialog(this);
    Dialog.setWindowModality(Qt::WindowModal);
    Dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (Dialog.exec() != QDialog::Accepted)
        return false;
    return SaveFile(Dialog.selectedFiles().first());
}

void MainWindow::About()
{
    /* HTML */
    QMessageBox::about(this, tr("About Application"),
                       tr("The <b>Application</b> example demonstrates "
                          "how to write modern GUI applications using Qt, "
                          "with a menu bar, tool bars, and a status bar."));
}

void MainWindow::DocumentWasModified()
{
    setWindowModified(TextEdit -> document() -> isModified());
}
