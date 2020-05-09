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

void MainWindow::CreateActions()
{
    QMenu * FileMenu = menuBar() -> addMenu(tr("&File"));
    QToolBar * FileToolBar = addToolBar(tr("File"));
    const QIcon NewIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    QAction * NewAct = new QAction(NewIcon, tr("&New"), this);
    NewAct -> setShortcuts(QKeySequence::New);
    NewAct -> setStatusTip(tr("Create a new file"));
    connect(NewAct, &QAction::triggered, this, &MainWindow::NewFile);
    FileMenu -> addAction(NewAct);
    FileToolBar -> addAction(NewAct);

    const QIcon OpenIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction * OpenAct = new QAction(OpenIcon, tr("&Open..."), this);
    OpenAct -> setShortcuts(QKeySequence::Open);
    OpenAct -> setStatusTip(tr("Open an existing file"));
    connect(OpenAct, &QAction::triggered, this, &MainWindow::Open);
    FileMenu -> addAction(OpenAct);
    FileToolBar -> addAction(OpenAct);

    const QIcon SaveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    QAction * SaveAct = new QAction(SaveIcon, tr("&Save"), this);
    SaveAct -> setShortcuts(QKeySequence::Save);
    SaveAct -> setStatusTip(tr("Save the document to disk"));
    connect(SaveAct, &QAction::triggered, this, &MainWindow::Save);
    FileMenu -> addAction(SaveAct);
    FileToolBar -> addAction(SaveAct);

    const QIcon SaveAsIcon = QIcon::fromTheme("document-save-as");
    QAction * SaveAsAct = FileMenu -> addAction(SaveAsIcon, tr("Save &As..."), this, &MainWindow::SaveAs);
    SaveAsAct -> setShortcuts(QKeySequence::SaveAs);
    SaveAsAct -> setStatusTip(tr("Save the document under a new name"));

    FileMenu -> addSeparator();

    const QIcon ExitIcon = QIcon::fromTheme("application-exit");
    QAction * ExitAct = FileMenu -> addAction(ExitIcon, tr("E&xit"), this, &QWidget::close);
    ExitAct -> setShortcuts(QKeySequence::Quit);
    ExitAct -> setStatusTip(tr("Exit the application"));

    QMenu * EditMenu = menuBar() -> addMenu(tr("&Edit"));
    QToolBar * EditToolBar = addToolBar(tr("Edit"));

#ifndef QT_NO_CLIPBOARD
    const QIcon CutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png"));
    QAction * CutAct = new QAction(CutIcon, tr("Cu&t"), this);
    CutAct -> setShortcuts(QKeySequence::Cut);
    CutAct -> setStatusTip(tr("Cut the current selection's contents to the clipboard"));
    connect(CutAct, &QAction::triggered, TextEdit, &QPlainTextEdit::cut);
    EditMenu -> addAction(CutAct);
    EditToolBar->addAction(CutAct);

    const QIcon CopyIcon = QIcon::fromTheme("edit-copy", QIcon(":/images/copy.png"));
    QAction * CopyAct = new QAction(CopyIcon, tr("&Copy"), this);
    CopyAct -> setShortcuts(QKeySequence::Copy);
    CopyAct -> setStatusTip(tr("Copy the current selection's contents to the clipboard"));
    connect(CopyAct, &QAction::triggered, TextEdit, &QPlainTextEdit::copy);
    EditMenu -> addAction(CopyAct);
    EditToolBar -> addAction(CopyAct);

    const QIcon PasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/images/paste.png"));
    QAction * PasteAct = new QAction(PasteIcon, tr("&Paste"), this);
    PasteAct -> setShortcuts(QKeySequence::Paste);
    PasteAct -> setStatusTip(tr("Paste the clipboard's contents into the current selection"));
    connect(PasteAct, &QAction::triggered, TextEdit, &QPlainTextEdit::paste);
    EditMenu -> addAction(PasteAct);
    EditToolBar -> addAction(PasteAct);

    menuBar() -> addSeparator();

#endif  /* !QT_NO_CLIPBOARD */

    QMenu * HelpMenu = menuBar() -> addMenu(tr("&Help"));
    QAction * AboutAct = HelpMenu -> addAction(tr("&About"), this, &MainWindow::About);
    AboutAct -> setStatusTip(tr("Show the application's About box"));

    QAction * AboutQtAct = HelpMenu -> addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    AboutQtAct -> setStatusTip(tr("Show the Qt library's About box"));

#ifndef QT_NO_CLIPBOARD
    CutAct -> setEnabled(false);
    CopyAct -> setEnabled(false);
    connect(TextEdit, &QPlainTextEdit::copyAvailable, CutAct, &QAction::setEnabled);
    connect(TextEdit, &QPlainTextEdit::copyAvailable, CopyAct, &QAction::setEnabled);
#endif
}

void MainWindow::CreateStatusBar()
{
    statusBar() -> showMessage(tr("Ready"));
}

void MainWindow::ReadSettings()
{
    QSettings Settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray Geometry = Settings.value("geometry", QByteArray()).toByteArray();
    if (Geometry.isEmpty())
    {
        const QRect AvailableGeometry = screen() -> availableGeometry();
        resize(AvailableGeometry.width() / 3, AvailableGeometry.height() / 2);
        move((AvailableGeometry.width() - width()) / 2,
             (AvailableGeometry.height() - height()) / 2);
    }
    else
        restoreGeometry(Geometry);
}

void MainWindow::WriteSettings()
{
    QSettings Settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    Settings.setValue("geometry", saveGeometry());
}

bool MainWindow::MaybeSave()
{
    if (!TextEdit -> document() -> isModified())
        return true;
    const QMessageBox::StandardButton Ret = QMessageBox::warning(this, tr("Application"),
                                                                 tr("The document has been modified.\n"
                                                                    "Do you want to save your changes?"),
                                                                 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (Ret)
    {
    case QMessageBox::Save:
        return Save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

void MainWindow::LoadFile(const QString & FileName)
{
    QFile File(FileName);
    if (!File.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Application"), tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(FileName), File.errorString()));
        return;
    }

    QTextStream In(&File);
#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    TextEdit -> setPlainText(In.readAll());
#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif

    SetCurrentFile(FileName);
    statusBar() -> showMessage(tr("File loaded"), 2000);    /* 2 Seconds */
}

bool MainWindow::SaveFile(const QString & FileName)
{
    QString ErrorMessage;

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    QSaveFile File(FileName);
    if (File.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream Out(&File);
        Out << TextEdit -> toPlainText();
        if (!File.commit())
            ErrorMessage = tr("Cannot write file %1:\n%2.")
                    .arg(QDir::toNativeSeparators(FileName), File.errorString());
    }
    else
        ErrorMessage = tr("Cannot open file %1 for writing:\n%2.")
                .arg(QDir::toNativeSeparators(FileName), File.errorString());

    QGuiApplication::restoreOverrideCursor();

    if (!ErrorMessage.isEmpty())
    {
        QMessageBox::warning(this, tr("Application"), ErrorMessage);
        return false;
    }

    SetCurrentFile(FileName);
    statusBar() -> showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::SetCurrentFile(const QString & FileName)
{
    CurFile = FileName;
    TextEdit -> document() -> setModified(false);
    setWindowModified(false);

    QString ShownName = CurFile;
    if (CurFile.isEmpty())
        ShownName = "untitled.txt";
    setWindowFilePath(ShownName);
}

QString MainWindow::StrippedName(const QString & FullFileName)
{
    return QFileInfo(FullFileName).fileName();
}
