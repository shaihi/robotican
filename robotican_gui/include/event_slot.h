#ifndef EVENT_SLOT_H
#define EVENT_SLOT_H
#include "robotican_gui.h"

#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <memory>
#include <ros/ros.h>
#include <QThread>
#include <QFuture>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <QtConcurrentRun>
#include "led.h"
#include "arm_move.h"
#include <QMessageBox>

#define TIMEOUT 1.0

class EventSlot : public QThread {

    enum Status {CANCELED, WORKING, SUCCESS, FAIL};

Q_OBJECT
public:
    EventSlot();
    ~EventSlot();
    void initiate(Ui::MainWindow &guiHandle, QApplication &app);
    void moveArm();

public Q_SLOTS:
    void setBatPwr(int val);
    void setLed(long int val, Led* led);
    void setMoveState(Status state);
    void closeApp();
    void moveArmToDrive();
    void moveArmToPreset();

private:
    Ui::MainWindow * _guiHandle;
    QApplication * _app;
    ros::NodeHandle _nHandle;
    ArmMove* _arm;
    std::string _targetName;
    std::string _userMsg;

    double calcTimeOut(long int startTime, long int endTime);
    bool execMove();
};


#endif //EVENT_SLOT_H