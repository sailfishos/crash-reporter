/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Riku Halonen <riku.halonen@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef CREPORTERCOREREGISTRY_H
#define CREPORTERCOREREGISTRY_H

// System includes

#include <QObject>
#include <QStringList>

// Forward declarations

class CReporterCoreRegistryPrivate;

/*!
 * @class CReporterCoreRegistry
 * @brief This class is a parent proxy class CReporterCoreDir instances.
 *
 * @sa CReporterCoreDir
 */
class CReporterCoreRegistry : public QObject 
{
	Q_OBJECT
		
public:
    static CReporterCoreRegistry *instance();

	/*!
     * @brief Class destructor.
	 *
	 */
	~CReporterCoreRegistry();

	 /*!
     * @brief This function searches for core dump files from directories and appends
	 * the file paths into list.
	 *  
     * @return List to be populated with rich core file paths.
	 * 
	 */
     QStringList collectAllCoreFiles() const;

	/*!
     * @brief This function returns a list of core directory paths.
	 * 
     * @return List of directory paths.
	 */
	QStringList getCoreLocationPaths();

	/*! 
     * @brief Checks directory for new core files.
     * 
     * @param path Reference to directory to be checked.
	 *	
     * @return Absolute path to core file. String is NULL, if new valid
	 *	core file was not found.
     */	
    QString checkDirectoryForCores(const QString& path);

public Q_SLOTS:
	/*!
      * @brief Parent can call this to refresh internal core file lists of
	  * CReporterCoreDir instances. Typically called, when monitoring
	  * is activated and file lists needs to updated.
	  */
	void refreshRegistry();

Q_SIGNALS:
	/*!
     * @brief This signal is emission is caused by mmc event.
	 *
	 */
	void coreLocationsUpdated();

	/*!
     * @brief Sent when refreshRegistry() is called.
	 *
	 */
	void registryRefreshNeeded();

private Q_SLOTS:
    /*!
     * @brief This function is called, when mmc gconf status changes.
     *
     * Timer is started to add some wait time. This is needed because gconf change events
     * come before mounting, but our actions need to be carried out after mounting.
     * After timer elapses, coreLocationsUpdated signal is emitted.
     *
     * @param key Changed setting.
     */
    void mmcStateChanged(const QString &key);

private:
    /**
     * Creates new core registry.
     *
     * @param parent the parent QObject
     */
    CReporterCoreRegistry(QObject *parent);
    Q_DISABLE_COPY(CReporterCoreRegistry)

    /*!
      * @brief Iniates registry and instantiates CReporterCoreDir objects.
      *
      */
	void createCoreLocationRegistry();
	
private: // data
		
    Q_DECLARE_PRIVATE(CReporterCoreRegistry)
    //! @arg Pointer to private data class.
    CReporterCoreRegistryPrivate *d_ptr;

#ifdef CREPORTER_UNIT_TEST
	friend class Ut_CReporterCoreRegistry;
#endif

};

#endif /* CREPORTERCOREREGISTRY_H */
