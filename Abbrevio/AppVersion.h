/// \file
/// \author Braeden V
///
/// \brief Declaration of the application version number class (major.minor.patch)
///
/// Licensed under the MIT License. See LICENSE file in the project root for full license information.


#ifndef ABBREVIO_APP_VERSION_H
#define ABBREVIO_APP_VERSION_H


//****************************************************************************************************************************************************
/// \brief A major.minor.patch version number. A default-constructed instance is invalid
//****************************************************************************************************************************************************
class AppVersion {
public: // static member functions
    static AppVersion fromString(QString const &str); ///< Find and parse a version number ("1.2" or "1.2.3") in a string

public: // member functions
    AppVersion() = default; ///< Default constructor, creates an invalid version number
    AppVersion(qint32 major, qint32 minor, qint32 patch = 0); ///< Constructor
    AppVersion(AppVersion const &) = default; ///< Copy constructor
    AppVersion(AppVersion &&) = default; ///< Move constructor
    ~AppVersion() = default; ///< Destructor
    AppVersion &operator=(AppVersion const &) = default; ///< Assignment operator
    AppVersion &operator=(AppVersion &&) = default; ///< Move assignment operator
    bool isValid() const; ///< Check if the version number is valid
    qint32 major() const; ///< Return the major version number
    qint32 minor() const; ///< Return the minor version number
    qint32 patch() const; ///< Return the patch version number
    QString toString() const; ///< Return the version number as a string ("major.minor.patch")
    bool operator==(AppVersion const &other) const; ///< Equality test. Invalid version numbers are never equal
    bool operator!=(AppVersion const &other) const; ///< Inequality test
    bool operator<(AppVersion const &other) const; ///< Strict inferiority test. False if either version number is invalid
    bool operator>(AppVersion const &other) const; ///< Strict superiority test. False if either version number is invalid

private: // data members
    qint32 major_ { -1 }; ///< The major version number
    qint32 minor_ { -1 }; ///< The minor version number
    qint32 patch_ { -1 }; ///< The patch version number
};


#endif // #ifndef ABBREVIO_APP_VERSION_H
