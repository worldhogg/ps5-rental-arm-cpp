#pragma once
#include <QString>

// Фото- и видеоматериалы внешнего состояния консоли при возврате.
// Записываются оператором вручную; решение о состоянии принимает специалист.
class ReturnMediaRecord {
public:
    ReturnMediaRecord();

    QString getPhotoPath() const;
    QString getVideoPath() const;
    QString getExternalCondition() const;
    QString getAdminComment() const;

    void setPhotoPath(const QString& path);
    void setVideoPath(const QString& path);
    void setExternalCondition(const QString& condition);
    void setAdminComment(const QString& comment);

    bool hasPhoto() const;
    bool hasVideo() const;
    bool isComplete() const;      // есть хотя бы фото или видео
    bool needsManualReview() const;

private:
    QString m_photoPath;
    QString m_videoPath;
    QString m_externalCondition;
    QString m_adminComment;
};
