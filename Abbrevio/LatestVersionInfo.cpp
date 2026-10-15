/// \file
/// \author Xavier Michelon
///
/// \brief Implementation of class holding information about the latest version of the application
///  
/// Copyright (c) Xavier Michelon. All rights reserved.  
/// Licensed under the MIT License. See LICENSE file in the project root for full license information.  


#include "stdafx.h"
#include "LatestVersionInfo.h"


namespace {
qint32 constexpr kHashLength = 32; /// The length in bytes of the checksum
QString const kPropTagName = "tag_name"; ///< The GitHub release property name for the tag name
QString const kPropName = "name"; ///< The GitHub release/asset property name for the name
QString const kPropHtmlUrl = "html_url"; ///< The GitHub release property name for the release page URL
QString const kPropBody = "body"; ///< The GitHub release property name for the release notes (markdown)
QString const kPropAssets = "assets"; ///< The GitHub release property name for the assets array
QString const kPropDownloadUrl = "browser_download_url"; ///< The GitHub asset property name for the download URL
QString const kPropDigest = "digest"; ///< The GitHub asset property name for the digest ("sha256:<hex>")
QString const kSha256DigestPrefix = "sha256:"; ///< The prefix of SHA256 digests in GitHub assets
QString const kInstallerSuffix = "-Installer.exe"; ///< The suffix of the installer asset name


//****************************************************************************************************************************************************
/// \param[in] assets The assets array of the GitHub release
/// \return The installer asset, or an empty object if none could be found
//****************************************************************************************************************************************************
QJsonObject findInstallerAsset(QJsonArray const &assets) {
    QJsonObject fallback;
    for (QJsonValue const &value: assets) {
        QJsonObject const asset = value.toObject();
        QString const name = asset[kPropName].toString();
        if (name.endsWith(kInstallerSuffix, Qt::CaseInsensitive))
            return asset;
        if (fallback.isEmpty() && name.endsWith(".exe", Qt::CaseInsensitive))
            fallback = asset;
    }
    return fallback;
}
}


//****************************************************************************************************************************************************
// 
//****************************************************************************************************************************************************
LatestVersionInfo::LatestVersionInfo()
    : versionNumber_() {
}


//****************************************************************************************************************************************************
/// If the JSON object does not contain valid info, the constructed LatestVersionInfo instance is not valid
///
/// \param[in] object The JSON document containing the version information
//****************************************************************************************************************************************************
LatestVersionInfo::LatestVersionInfo(QJsonObject const &object)
    : LatestVersionInfo() {
    this->parseFromJsonObject(object);
}


//****************************************************************************************************************************************************
/// \return true if and only if the latest version information is not valid
//****************************************************************************************************************************************************
bool LatestVersionInfo::isValid() const {
    return versionNumber_.isValid() && (!downloadUrl_.isEmpty()) && (sha256Hash_.size() == kHashLength);
}


//****************************************************************************************************************************************************
/// \return The version number.
//****************************************************************************************************************************************************
AppVersion LatestVersionInfo::versionNumber() const {
    return versionNumber_;
}


//****************************************************************************************************************************************************
/// \param[in] versionNumber The version number.
//****************************************************************************************************************************************************
void LatestVersionInfo::setVersionNumber(AppVersion const &versionNumber) {
    versionNumber_ = versionNumber;
}


//****************************************************************************************************************************************************
/// \return The download URL for the installer
//****************************************************************************************************************************************************
QString LatestVersionInfo::downloadUrl() const {
    return downloadUrl_;
}


//****************************************************************************************************************************************************
/// \param[in] downloadUrl The download download URL for the installer
//****************************************************************************************************************************************************
void LatestVersionInfo::setDownloadUrl(QString const &downloadUrl) {
    downloadUrl_ = downloadUrl;
}


//****************************************************************************************************************************************************
/// \return The release URL
//****************************************************************************************************************************************************
QString LatestVersionInfo::releaseUrl() const {
    return releaseUrl_;
}


//****************************************************************************************************************************************************
/// \param[in] url The release url
//****************************************************************************************************************************************************
void LatestVersionInfo::setReleaseUrl(QString const &url) {
    releaseUrl_ = url;
}


//****************************************************************************************************************************************************
/// \return the SHA256 has for the installer
//****************************************************************************************************************************************************
QByteArray LatestVersionInfo::sha256Hash() const {
    return sha256Hash_;
}


//****************************************************************************************************************************************************
/// \param[in] sha256Hash The SHA256 hash
//****************************************************************************************************************************************************
void LatestVersionInfo::setSha256Hash(QByteArray const &sha256Hash) {
    sha256Hash_ = sha256Hash;
}


//****************************************************************************************************************************************************
/// \return The release notes, in markdown format
//****************************************************************************************************************************************************
QString LatestVersionInfo::releaseNotes() const {
    return releaseNotes_;
}


//****************************************************************************************************************************************************
/// \param[in] releaseNotes The release notes
//****************************************************************************************************************************************************
void LatestVersionInfo::setReleaseNotes(QString const &releaseNotes) {
    releaseNotes_ = releaseNotes;
}


//****************************************************************************************************************************************************
/// If the JSON object does not contain valid info, the constructed LatestVersionInfo instance is not valid
///
/// \param[in] object The JSON object describing a GitHub release, as returned by the GitHub REST API
//****************************************************************************************************************************************************
void LatestVersionInfo::parseFromJsonObject(QJsonObject const &object) {
    versionNumber_ = AppVersion::fromString(object[kPropTagName].toString());
    if (!versionNumber_.isValid()) // the tag is not a version number (e.g. "beta"), we try the release name
        versionNumber_ = AppVersion::fromString(object[kPropName].toString());
    releaseUrl_ = object[kPropHtmlUrl].toString();
    releaseNotes_ = object[kPropBody].toString();
    QJsonObject const installer = findInstallerAsset(object[kPropAssets].toArray());
    downloadUrl_ = installer[kPropDownloadUrl].toString();
    QString const digest = installer[kPropDigest].toString();
    if (digest.startsWith(kSha256DigestPrefix, Qt::CaseInsensitive))
        sha256Hash_ = QByteArray::fromHex(digest.mid(kSha256DigestPrefix.size()).toLatin1());
}


