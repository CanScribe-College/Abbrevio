/// \file
/// \author Xavier Michelon
///
/// \brief Implementation of worker responsible for checking for updates
///  
/// Copyright (c) Xavier Michelon. All rights reserved.  
/// Licensed under the MIT License. See LICENSE file in the project root for full license information.  


#include "stdafx.h"
#include "UpdateCheckWorker.h"
#include "AbbrevioGlobals.h"
#include "AbbrevioConstants.h"
#include <XMiLib/Exception.h>


namespace {
#ifdef NDEBUG
/// The GitHub API URL of the latest stable release (pre-releases and drafts are excluded)
QString const kLatestReleaseUrl = "https://api.github.com/repos/CanScribe-College/Abbrevio/releases/latest";
#else
/// The GitHub API URL of the most recent releases, including pre-releases. The first item is the most recent
QString const kLatestReleaseUrl = "https://api.github.com/repos/CanScribe-College/Abbrevio/releases?per_page=1";
#endif
}


//****************************************************************************************************************************************************
/// \param[in] parent The parent object of the instance
//****************************************************************************************************************************************************
UpdateCheckWorker::UpdateCheckWorker(QObject *parent, AppVersion const &skipVersionNumber)
    : QObject(parent)
    , skipVersionNumber_(skipVersionNumber) {

}


//****************************************************************************************************************************************************
// 
//****************************************************************************************************************************************************
void UpdateCheckWorker::run() {
    this->performUpdateCheck();
    emit finished(); // note that finished is emitted even in case of error
}


//****************************************************************************************************************************************************
///
//****************************************************************************************************************************************************
void UpdateCheckWorker::performUpdateCheck() {
    try {
        xmilib::DebugLog &log = globals::debugLog();
        log.addInfo(QString("Update check started. Installed version is Abbrevio %1.")
            .arg(constants::kVersionNumber.toString()));
        QByteArray const jsonData = this->downloadLatestVersionInformation();
        SpLatestVersionInfo const latestVersionInfo = this->parseJsonData(jsonData);
        log.addInfo(QString("Downloaded latest version information. Latest version is Abbrevio %1.")
            .arg(latestVersionInfo->versionNumber().toString()));
        bool skipped = false;
        if (isNewVersionAvailable(latestVersionInfo, &skipped)) {
            log.addInfo("A new version is available.");
            emit updateIsAvailable(latestVersionInfo);
        } else {
            if (skipped)
                log.addInfo("A new version was available, but the user had previously decided to skip it.");
            else
                log.addInfo("No new version is available.");
            emit noUpdateIsAvailable();
        }
        log.addInfo("Update check ended.");
    }
    catch (xmilib::Exception const &e) {
        emit error(e.qwhat());
    }
}


//****************************************************************************************************************************************************
/// \return The downloaded data
//****************************************************************************************************************************************************
QByteArray UpdateCheckWorker::downloadLatestVersionInformation() const {
    QNetworkAccessManager nam;
    QEventLoop loop;
    QNetworkRequest request(kLatestReleaseUrl);
    request.setRawHeader("Accept", "application/vnd.github+json");
    request.setHeader(QNetworkRequest::UserAgentHeader, QString("%1/%2").arg(constants::kApplicationName)
        .arg(constants::kVersionNumber.toString())); // the GitHub API rejects requests without a user agent
    QNetworkReply *reply = nam.get(request);
    QString errorMessage;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(reply, &QNetworkReply::errorOccurred, [&](QNetworkReply::NetworkError code) {
        errorMessage = QString("Error %1: %2").arg(code).arg(reply->errorString());
    });
    loop.exec();
    if (!errorMessage.isEmpty())
        throw xmilib::Exception(QString("Could not retrieve the latest release information from GitHub: %1")
            .arg(errorMessage));
    QByteArray result = reply->readAll();
    reply->deleteLater();
    return result;
}


//****************************************************************************************************************************************************
/// \param[in] jsonData the JSON data
/// \return A shared pointer to the parsed data. 
//****************************************************************************************************************************************************
SpLatestVersionInfo UpdateCheckWorker::parseJsonData(QString const &jsonData) const {
    QString errMsg;
    QJsonParseError error {};
    QJsonDocument const doc = QJsonDocument::fromJson(jsonData.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError)
        throw xmilib::Exception(QString("Could not retrieve version information. The downloaded JSON file is "
                                        "invalid: %1").arg(error.errorString()));
    // the release list endpoint returns an array, most recent release first. The latest release endpoint returns an object
    if (doc.isArray() && doc.array().isEmpty())
        throw xmilib::Exception("Could not retrieve version information. No release was found on GitHub.");
    QJsonObject const release = doc.isArray() ? doc.array().first().toObject() : doc.object();
    SpLatestVersionInfo result = std::make_shared<LatestVersionInfo>(release);
    if (!result->isValid())
        throw xmilib::Exception(QString("Could not retrieve version information. The GitHub release does not have "
                                        "a version number in its tag or name, or an installer asset with a SHA256 digest."));
    return result;
}


//****************************************************************************************************************************************************
/// \param[in] latestVersionInfo The latest version information.
/// \param[out] outSkipped If not null, and the function returns false, on exit the value pointed will be true
/// if the update was skipped by the user.
/// \return true if and only if an update is available.
//****************************************************************************************************************************************************
bool UpdateCheckWorker::isNewVersionAvailable(SpLatestVersionInfo const &latestVersionInfo, bool *outSkipped) const {
    if (outSkipped)
        *outSkipped = false;
    if (!latestVersionInfo)
        throw xmilib::Exception("Could not check for new version: the retrieved latest version information is "
                                "null.");
    if (skipVersionNumber_.isValid() && latestVersionInfo->versionNumber().isValid()
        && (skipVersionNumber_ == latestVersionInfo->versionNumber())) {
        if (outSkipped)
            *outSkipped = true;
        return false;

    }

    return latestVersionInfo->versionNumber() > constants::kVersionNumber;
}


