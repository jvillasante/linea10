#ifndef ENROLLDIALOG_H
#define ENROLLDIALOG_H

#include "soaphandler.h"
#include "workersensormulti.h"

#include <QDialog>
#include <QSettings>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTimer>

class EnrollDialog : public QDialog
{
  Q_OBJECT
  public:
    explicit EnrollDialog(QWidget *parent, QSettings *settings, WorkerSensorMulti *workerSensor);
    ~EnrollDialog();

  private slots:
    void btn1Clicked();
    void btn2Clicked();
    void btn3Clicked();
    void btn4Clicked();
    void btn5Clicked();
    void btn6Clicked();
    void btn7Clicked();
    void btn8Clicked();
    void btn9Clicked();
    void btn0Clicked();
    void btnHyphenClicked();
    void btnKClicked();
    void btnDeleteClicked();
    void btnOkClicked();
    void btnCancelClicked();

  protected:
    virtual void closeEvent (QCloseEvent *e);

  private:
    void initializeUI();
    void setScreen2(QString, QString, QString);
    void closeDialog(QString msg);

  private:
    QString rutNumber;
    QSettings *settings;
    int screen;  // 1: keypad, 2: user data, 3: user fingerprint
    QString imgRawB64, imgbaseJpg64, template1;

    SoapHandler *soapHandlerPersona;
    SoapHandler *soapHandlerEnroll;

    WorkerSensorMulti *workerSensor;

    QGridLayout *grid;
    QPushButton *btnOk;
    QPushButton *btnCancel;

    // screen 1
    QLineEdit *edtRut;
    QLabel *lblMsg;
    QPushButton *btn1;
    QPushButton *btn2;
    QPushButton *btn3;
    QPushButton *btn4;
    QPushButton *btn5;
    QPushButton *btn6;
    QPushButton *btn7;
    QPushButton *btn8;
    QPushButton *btn9;
    QPushButton *btn0;
    QPushButton *btnHyphen;
    QPushButton *btnK;
    QPushButton *btnDelete;

    // screen 2
    QLabel *lblEmpresa;
    QLabel *lblName;
    QLabel *lblMsgScreen2;

    QTimer *closeTimer;

  private:
    void verificaPersona();
    bool parseXmlPersona(QString &response, QString &identifier, QString &rut, QString &name);

    void enroll(QString &identificacion, QString &imgRawB64, QString &imgJpgB64, QString &tmp1);
    bool parseXmlEnroll(QString &response, QString &identificador, QString &serial, QString &resultado);

  signals:
    void enrollDialogClosed(QString);

  private slots:
    void verificaPersonaFinished(QString);
    void verificaPersonaError(QString);
    void enrollWorkDone(uchar *compositeImage, uchar *templateImage, int width, int height);

    void enrollFinished(QString);
    void enrollError(QString);

    void timerCloseDialog();
};

#endif // ENROLLDIALOG_H
