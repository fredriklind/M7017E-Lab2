#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QHostAddress>
#include "videoserver.h"
#include <QNetworkInterface>
#include <QMap>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QScreen>
#include <QGuiApplication>
#include <QTcpSocket>
#include <QDialog>
#include <QShortcut>

#define COMMUNICATION_PORT 5000
#define BASE_PORT 6000
#define SLOT_SIZE 10

// The listen command!
// GST_DEBUG=2,udpsrc:5 gst-launch-0.10 -v udpsrc port=6002 ! application/x-rtp, payload=127 ! rtph264depay ! ffdec_h264 ! autovideosink
// gst-launch-0.10 videotestsrc ! application/x-rtp, payload=127 ! rtph264depay ! ffdec_h264 ! autovideosink

/**
 * @brief Sets up the interface, starts text communication by starting a Client and a Server. Also starts the video
 * communication part by starting a videoClient.
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Text communication
    client = new Client(this);
    server = new Server(this);
    server->listen();
    connect(client, SIGNAL(couldNotConnect(QString)), this, SLOT(couldNotConnectToCallee(QString)));
    connect(server, SIGNAL(didReceiveMessage(QString)), this, SLOT(serverDidReceiveMessage(QString)));

    // Video communication
    videoClient = new VideoClient();
    videoServer = new VideoServer();

    // Get local IP
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()){
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            myIP = address.toString();
    }

    // Setup the ui
    ui->ipField->setToolTip("Your IP: " + myIP);
    ui->ipField->setPlaceholderText("IP Address");
    ui->messageField->setPlaceholderText("Overlay message");
    this->setWindowTitle(" ");

    // Setup participant list
    participantListWindow = new QDialog;
    participantListWindow->setContentsMargins(20,20,20,20);
    participantListWindow->setWindowTitle("Participants");
    participantListWidget = new QListWidget(participantListWindow);
    participantListWindow->setFixedSize(210, 240);
    participantListWidget->setFixedSize(210, 240);
    participantListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    participantListWidget->addItem(myIP + " (Thats you!)");
    QShortcut *sq = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_P), this, SLOT(toggleParticipantListWindow()));

    // Setup video layout
    QHBoxLayout *layout = new QHBoxLayout;
    ui->videoWidget->setStyleSheet("background:black;");
    ui->videoWidget->setLayout(layout);
    videoServer->init(addVideoToInterface());

    rescaleWindow();
    participants = QStringList();

    myTempSlot = 0;
}

/**
 * @brief Adds a new QWidget to the video area of the interface
 * @return The Window ID of the newly inserted QWidget
 */
WId MainWindow::addVideoToInterface()
{
    QWidget *w = new QWidget();
    ui->videoWidget->layout()->addWidget(w);
    w->show();
    rescaleWindow();
    return w->winId();
}

/**
 * @brief Hides and shows the participant list window
 */
void MainWindow::toggleParticipantListWindow()
{
    qDebug() << "Testar";
    if(participantListWindow->isVisible()){
        participantListWindow->hide();
    } else {
        participantListWindow->show();
    }
}

/**
 * @brief This slot is called when the text communication server receives a message, it processes the message and delegates it.
 * @param str The string that was received from the sending Client
 */
void MainWindow::serverDidReceiveMessage(QString str)
{
    QByteArray ba(str.toStdString().c_str());
    QJsonObject obj = QJsonDocument::fromJson(ba).object();
    qDebug() << "RECEIVED THIS: ";
    qDebug() << QJsonDocument::fromJson(ba).toJson();
    QVariantMap arr = obj.toVariantMap();
    delegateMessage(arr);
}

/**
 * @brief This methods handles the command that the Server has received. There are two types of commands that can be received.
 * "initiate-call" is received when another Client wants to start a conversation. "update-participants" is received when another
 * Client has new information about the participants in the call.
 * @param arr The array containing the command, and its parameters
 */
void MainWindow::delegateMessage(QVariantMap arr)
{
    QString command = arr["command"].toString();

    if(command == "initiate-call"){
        // An empty participants list means that we are currently not in a call
        if(participants.isEmpty()){
            // This means that the call we are asked to join a call that already contains participants
            if(arr.contains("participants")){
                QStringList currentParticipants = arr["participants"].toString().split(",");
                setParticipants(currentParticipants);
            }

            // Add ourself to the participants
            addParticipant(myIP);
        }

        // Add caller to participant list
        addParticipant(arr["sender-ip"].toString());

        // Send update-participants command to all participants, excluding ourselves
        client->sendUpdateToParticipants(participants, myIP);
    }

    if(command == "update-participants"){
        // Set local participant array to received one
        setParticipants(arr["participants"].toString().split(","));
    }
}

/**
 * @brief Called when the callbutton is clicked. Checks if the IP provided in the ipField is valid (inside the sendMessage method),
 * then constructs an "initiate-call" command and sends it to that IP. If we are currently in a call
 * the current participants of the call are included in the command.
 */
void MainWindow::on_callButton_clicked()
{
    if(ui->ipField->text() == "127.0.0.1" || ui->ipField->text() == "localhost"){
        QMessageBox msgBox;
        msgBox.setWindowTitle("Error!");
        msgBox.setText("Nope, you can't call yourself. Sorry.");
        msgBox.exec();
        return;
    }

    // Construct request
    QVariantMap request;
    request["command"] = "initiate-call";
    if(!participants.isEmpty()){
        request["participants"] = participants.join(",");
    }
    QHostAddress ip(ui->ipField->text());

    qDebug() << participants;
    // Send call initiation command
    client->sendMessage(request, ip);
    qDebug() << participants;
}

/**
 * @brief Called when there was an error connecting to the IP provided in the ipField, shows an alert box.
 * @param error
 */
void MainWindow::couldNotConnectToCallee(QString error)
{
    QMessageBox msgBox;
    msgBox.setText("Could not contact callee: " + error);
    msgBox.exec();
}

/**
 * @brief Tells the VideoServer to change the text overlay on the video that is being sent to the text in the
 * messageField field.
 * @param arg1 The current text in the field.
 */
void MainWindow::on_messageField_textChanged(const QString &arg1)
{
    videoServer->setTextOverlay(arg1);
}

/**
 * @brief Adds a participant to the local participant list and sets up listening and sending to and from that participant.
 * @param ip The ip to add to the local participant list.
 */
void MainWindow::addParticipant(QString ip)
{
    if(!participants.contains(ip)){
    participants.append(ip);

    // Start sending and listening to new newly added participant
    if(ip != myIP){
        int sendPort = getPortNumberForConnectionBetweenSlots(mySlot(), participants.indexOf(ip));
        int listenPort = getPortNumberForConnectionBetweenSlots(participants.indexOf(ip), mySlot());
        videoServer->sendToNewClient(ip, sendPort);
        videoClient->addListenPort(listenPort, addVideoToInterface());
        participantListWidget->addItem(ip);
    }
    qDebug() << "Added participant" + ip;
    }
}

/**
 * @brief Removes a participant from the local participant list. Not currently used.
 * @param ip The IP to remove from the local participant list.
 */
void MainWindow::removeParticipant(QString ip)
{
    if(participants.removeOne(ip)){
        qDebug() << "Did remove participant";
    } else {
        qDebug() << "Could not remove: Participant not in list";
    }
}

/**
 * @brief Sets the local participant list to a received one. This triggers new connections to be set up.
 * @param newParticipantList
 */
void MainWindow::setParticipants(QStringList newParticipantList)
{
    myTempSlot = newParticipantList.indexOf(myIP);
    if(myTempSlot == -1){
        myTempSlot = newParticipantList.count();
    }
    foreach(QString ip, newParticipantList){
        if(!participants.contains(ip)) {
            addParticipant(ip);
        }
    }
}

/**
 * @brief Get the slot of the current client in the participant list
 * @return The slot (0,1,2...)
 */
int MainWindow::mySlot(){
    if(participants.contains(myIP)){
        return participants.indexOf(myIP);
    } else {
        qDebug() << "Im not in the participants list, returning temp slot!";
        return myTempSlot;
    }
}

/**
 * @brief Recalculate and set width and center the application window on the screen
 */
void MainWindow::rescaleWindow()
{
    int newWidth = 370+participants.count()*480;
    QScreen *screen = QApplication::screens().first();
    if( newWidth < screen->size().width()){
        this->resize(newWidth,this->height());
        this->move(QApplication::screens().first()->geometry().center() - this->rect().center());
    }
}

/**
 * @brief Calculates port mapping based on list positions. Slots are list positions with their alloted SLOT_SIZE.
 * @param fromSlot The sending slot.
 * @param toSlot The receiving slot.
 * @return The port that should be used for the communication from fromSlot to toSlot.
 */
int MainWindow::getPortNumberForConnectionBetweenSlots(int fromSlot, int toSlot)
{
    return BASE_PORT + SLOT_SIZE*fromSlot + toSlot;
}

/**
 * @brief Frees the ui from memory
 */
MainWindow::~MainWindow()
{
    delete ui;
}
