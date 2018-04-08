/*  Ekos Scheduler Job
    Copyright (C) Jasem Mutlaq <mutlaqja@ikarustech.com>

    This application is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
 */

#pragma once

#include "skypoint.h"

#include <QUrl>
#include <QMap>

class QTableWidgetItem;

class dms;

class SchedulerJob
{
  public:
    SchedulerJob();

    typedef enum {
        JOB_IDLE,
        JOB_EVALUATION,
        JOB_SCHEDULED,
        JOB_BUSY,
        JOB_ERROR,
        JOB_ABORTED,
        JOB_INVALID,
        JOB_COMPLETE
    } JOBStatus;
    typedef enum {
        STAGE_IDLE,
        STAGE_SLEWING,
        STAGE_SLEW_COMPLETE,
        STAGE_FOCUSING,
        STAGE_FOCUS_COMPLETE,
        STAGE_ALIGNING,
        STAGE_ALIGN_COMPLETE,
        STAGE_RESLEWING,
        STAGE_RESLEWING_COMPLETE,
        STAGE_POSTALIGN_FOCUSING,
        STAGE_POSTALIGN_FOCUSING_COMPLETE,
        STAGE_GUIDING,
        STAGE_GUIDING_COMPLETE,
        STAGE_CAPTURING,
        STAGE_COMPLETE
    } JOBStage;

    typedef enum { START_ASAP, START_CULMINATION, START_AT } StartupCondition;

    typedef enum { FINISH_SEQUENCE, FINISH_REPEAT, FINISH_LOOP, FINISH_AT } CompletionCondition;

    typedef enum {
        USE_NONE  = 0,
        USE_TRACK = 1 << 0,
        USE_FOCUS = 1 << 1,
        USE_ALIGN = 1 << 2,
        USE_GUIDE = 1 << 3
    } StepPipeline;

    QString getName() const;
    void setName(const QString &value);

    SkyPoint const & getTargetCoords() const;
    void setTargetCoords(dms& ra, dms& dec);

    StartupCondition getStartupCondition() const;
    void setStartupCondition(const StartupCondition &value);

    QDateTime getStartupTime() const;
    void setStartupTime(const QDateTime &value);

    QUrl getSequenceFile() const;
    void setSequenceFile(const QUrl &value);

    QUrl getFITSFile() const;
    void setFITSFile(const QUrl &value);

    double getMinAltitude() const;
    void setMinAltitude(const double &value);

    double getMinMoonSeparation() const;
    void setMinMoonSeparation(const double &value);

    bool getEnforceWeather() const;
    void setEnforceWeather(bool value);

    QDateTime getCompletionTime() const;
    void setCompletionTime(const QDateTime &value);

    CompletionCondition getCompletionCondition() const;
    void setCompletionCondition(const CompletionCondition &value);

    int getSequenceCount() const;
    void setSequenceCount(const int count);

    int getCompletedCount() const;
    void setCompletedCount(const int count);

    StepPipeline getStepPipeline() const;
    void setStepPipeline(const StepPipeline &value);

    JOBStatus getState() const;
    void setState(const JOBStatus &value);

    int getScore() const;
    void setScore(int value);

    int16_t getCulminationOffset() const;
    void setCulminationOffset(const int16_t &value);

    QTableWidgetItem *getStartupCell() const;
    void setStartupCell(QTableWidgetItem *value);

    void setDateTimeDisplayFormat(const QString &value);

    JOBStage getStage() const;
    void setStage(const JOBStage &value);

    StartupCondition getFileStartupCondition() const;
    void setFileStartupCondition(const StartupCondition &value);

    int64_t getEstimatedTime() const;
    void setEstimatedTime(const int64_t &value);

    bool getInSequenceFocus() const;
    void setInSequenceFocus(bool value);

    uint8_t getPriority() const;
    void setPriority(const uint8_t &value);

    bool getEnforceTwilight() const;
    void setEnforceTwilight(bool value);

    /* QTableWidgetItem *getNameCell() const; */
    void setNameCell(QTableWidgetItem *cell);

    /* QTableWidgetItem *getStatusCell() const; */
    void setStatusCell(QTableWidgetItem *cell);

    /* QTableWidgetItem *getStageCell() const; */
    void setStageCell(QTableWidgetItem *cell);

    QTableWidgetItem *getEstimatedTimeCell() const;
    void setEstimatedTimeCell(QTableWidgetItem *value);

    QTableWidgetItem *getCaptureCountCell() const;
    void setCaptureCountCell(QTableWidgetItem *value);

    QTableWidgetItem *getScoreCell() const;
    void setScoreCell(QTableWidgetItem *value);

    bool getLightFramesRequired() const;
    void setLightFramesRequired(bool value);

    uint16_t getRepeatsRequired() const;
    void setRepeatsRequired(const uint16_t &value);

    uint16_t getRepeatsRemaining() const;
    void setRepeatsRemaining(const uint16_t &value);

    QMap<QString, uint16_t> getCapturedFramesMap() const;
    void setCapturedFramesMap(const QMap<QString, uint16_t> &value);

    /** @brief Compare ::SchedulerJob instances based on score.
     * @arg a, b are ::SchedulerJob instances to compare.
     * @return true if the score of a is lower than the score of b.
     * @return false if the score of a is higher than or equal to the score of b.
     */
    static bool ascendingScore(SchedulerJob const *a, SchedulerJob const *b);

    /** @brief Compare ::SchedulerJob instances based on priority.
     * @arg a, b are ::SchedulerJob instances to compare.
     * @return true if the priority of b is lower than the priority of a.
     * @return false if the priority of b is higher than or equal to the priority of a.
     */
    static bool descendingPriority(SchedulerJob const *a, SchedulerJob const *b);

    /** @brief Compare ::SchedulerJob instances based on altitude.
     * @arg a, b are ::SchedulerJob instances to compare.
     * @return true if the altitude of b is lower than the altitude of a.
     * @return false if the altitude of b is higher than or equal to the altitude of a.
     */
    static bool descendingAltitude(SchedulerJob const *a, SchedulerJob const *b);

private:
    QString name;
    SkyPoint targetCoords;
    JOBStatus state { JOB_IDLE };
    JOBStage stage { STAGE_IDLE };

    StartupCondition fileStartupCondition { START_ASAP };
    StartupCondition startupCondition { START_ASAP };
    CompletionCondition completionCondition { FINISH_SEQUENCE };

    int sequenceCount { 0 };
    int completedCount { 0 };

    QDateTime startupTime;
    QDateTime completionTime;

    QUrl sequenceFile;
    QUrl fitsFile;

    double minAltitude { -1 };
    double minMoonSeparation { -1 };

    bool enforceWeather { false };
    bool enforceTwilight { false };

    StepPipeline stepPipeline { USE_NONE };

    QTableWidgetItem *nameCell { nullptr };
    QTableWidgetItem *statusCell { nullptr };
    QTableWidgetItem *stageCell { nullptr };
    QTableWidgetItem *startupCell { nullptr };
    QTableWidgetItem *estimatedTimeCell { nullptr };
    QTableWidgetItem *captureCountCell { nullptr };
    QTableWidgetItem *scoreCell { nullptr };

    void updateJobCell();

    int score { 0 };
    int16_t culminationOffset { 0 };
    uint8_t priority { 10 };
    int64_t estimatedTime { -1 };
    uint16_t repeatsRequired { 0 };
    uint16_t repeatsRemaining { 0 };
    bool inSequenceFocus { false };

    QString dateTimeDisplayFormat;

    bool lightFramesRequired { false };

    QMap<QString, uint16_t> capturedFramesMap;
};
