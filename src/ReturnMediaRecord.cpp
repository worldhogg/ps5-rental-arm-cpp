#include "ReturnMediaRecord.h"

ReturnMediaRecord::ReturnMediaRecord()
    : m_externalCondition("Без повреждений")
{}

QString ReturnMediaRecord::getPhotoPath() const       { return m_photoPath; }
QString ReturnMediaRecord::getVideoPath() const       { return m_videoPath; }
QString ReturnMediaRecord::getExternalCondition() const { return m_externalCondition; }
QString ReturnMediaRecord::getAdminComment() const    { return m_adminComment; }

void ReturnMediaRecord::setPhotoPath(const QString& path)           { m_photoPath = path; }
void ReturnMediaRecord::setVideoPath(const QString& path)           { m_videoPath = path; }
void ReturnMediaRecord::setExternalCondition(const QString& cond)   { m_externalCondition = cond; }
void ReturnMediaRecord::setAdminComment(const QString& comment)     { m_adminComment = comment; }

bool ReturnMediaRecord::hasPhoto() const { return !m_photoPath.isEmpty(); }
bool ReturnMediaRecord::hasVideo() const { return !m_videoPath.isEmpty(); }

bool ReturnMediaRecord::isComplete() const {
    return hasPhoto() || hasVideo();
}

bool ReturnMediaRecord::needsManualReview() const {
    return m_externalCondition == "Требуется ручная проверка";
}
