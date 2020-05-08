#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSessionManager>
#include <QPlainTextEdit>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    void LoadFile(const QString &);

protected:
    void closeEvent(QCloseEvent *) override;

private slots:
    void NewFile();
    void Open();
    bool Save();
    bool SaveAs();
    void About();
    void DocumentWasModified();
#ifndef QT_NO_SESSIONMANAGER
    void CommitData(QSessionManager &);
#endif

private:
    void CreateActions();
    void CreateStatusBar();
    void ReadSettings();
    void WriteSettings();
    bool MaybeSave();
    bool SaveFile(const QString &);
    void SetCurrentFile(const QString &);
    QString StrippedName(const QString &);

    QPlainTextEdit * TextEdit;
    QString CurFile;
};
#endif // MAINWINDOW_H
