/// \file
/// \author Braeden V
///
/// \brief Implementation of the application version number class (major.minor.patch)
///
/// Licensed under the MIT License. See LICENSE file in the project root for full license information.


#include "stdafx.h"
#include "AppVersion.h"
#include <tuple>


//****************************************************************************************************************************************************
/// \param[in] str The string to search for a version number (e.g. "v1.2.3", "1.2" or "Abbrevio v0.9")
/// \return The version number. If no version number was found, the returned instance is invalid. A missing patch
/// number is considered to be 0
//****************************************************************************************************************************************************
AppVersion AppVersion::fromString(QString const &str) {
    QRegularExpressionMatch const match = QRegularExpression(R"((\d+)\.(\d+)(?:\.(\d+))?)").match(str);
    if (!match.hasMatch())
        return AppVersion();
    return AppVersion(match.captured(1).toInt(), match.captured(2).toInt(), match.captured(3).toInt()); // empty capture gives 0
}


//****************************************************************************************************************************************************
/// \param[in] major The major version number
/// \param[in] minor The minor version number
/// \param[in] patch The patch version number
//****************************************************************************************************************************************************
AppVersion::AppVersion(qint32 major, qint32 minor, qint32 patch)
    : major_(major)
    , minor_(minor)
    , patch_(patch) {
}


//****************************************************************************************************************************************************
/// \return true if and only if the version number is valid
//****************************************************************************************************************************************************
bool AppVersion::isValid() const {
    return (major_ >= 0) && (minor_ >= 0) && (patch_ >= 0);
}


//****************************************************************************************************************************************************
/// \return The major version number
//****************************************************************************************************************************************************
qint32 AppVersion::major() const {
    return major_;
}


//****************************************************************************************************************************************************
/// \return The minor version number
//****************************************************************************************************************************************************
qint32 AppVersion::minor() const {
    return minor_;
}


//****************************************************************************************************************************************************
/// \return The patch version number
//****************************************************************************************************************************************************
qint32 AppVersion::patch() const {
    return patch_;
}


//****************************************************************************************************************************************************
/// \return A string representation of the version number
//****************************************************************************************************************************************************
QString AppVersion::toString() const {
    return QString("%1.%2.%3").arg(major_).arg(minor_).arg(patch_);
}


//****************************************************************************************************************************************************
/// \param[in] other The other version number
/// \return true if and only if both version numbers are valid and equal
//****************************************************************************************************************************************************
bool AppVersion::operator==(AppVersion const &other) const {
    return this->isValid() && other.isValid() && (std::tie(major_, minor_, patch_) == std::tie(other.major_, other.minor_, other.patch_));
}


//****************************************************************************************************************************************************
/// \param[in] other The other version number
/// \return true if and only if the version numbers are not equal
//****************************************************************************************************************************************************
bool AppVersion::operator!=(AppVersion const &other) const {
    return !(*this == other);
}


//****************************************************************************************************************************************************
/// \param[in] other The other version number
/// \return true if and only if both version numbers are valid and this version number is lower than other
//****************************************************************************************************************************************************
bool AppVersion::operator<(AppVersion const &other) const {
    return this->isValid() && other.isValid() && (std::tie(major_, minor_, patch_) < std::tie(other.major_, other.minor_, other.patch_));
}


//****************************************************************************************************************************************************
/// \param[in] other The other version number
/// \return true if and only if both version numbers are valid and this version number is greater than other
//****************************************************************************************************************************************************
bool AppVersion::operator>(AppVersion const &other) const {
    return other < *this;
}
