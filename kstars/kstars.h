/***************************************************************************
                          kstars.h  -  K Desktop Planetarium
                             -------------------
    begin                : Mon Feb  5 01:11:45 PST 2001
    copyright            : (C) 2001 by Jason Harris
    email                : jharris@30doradus.org
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KSTARS_H_
#define KSTARS_H_

#include <QtDBus/QtDBus>
#include <kxmlguiwindow.h>


#include "tools/observinglist.h"

// forward declaration is enough. We only need pointers
class QPalette;
class KActionMenu;

class dms;
class KStarsData;
class KStarsSplash;
class SkyMap;
class GeoLocation;
class FindDialog;
class InfoBoxes;
class TimeStepBox;

class INDIMenu;
class INDIDriver;
class imagesequence;
class AltVsTime;
class LCGenerator;
class WUTDialog;
class ScriptBuilder;
class PlanetViewer;
class JMoonTool;
class ImageViewer;

/**
	*@class KStars
	*@short This is the main window for KStars.
	*In addition to the GUI elements, the class contains the program clock,
	*KStarsData, and SkyMap objects.  It also contains functions for the DCOP interface.
	*@author Jason Harris
	*@version 1.0
	*/

class KStars : public KXmlGuiWindow
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kstars")

public:
    /**
    	*@short Constructor.
    	*@param doSplash should the splash panel be displayed during
    	*initialization.
    	*@param startClockRunning should the clock be running on startup?
    	*@param startDateString date (in string representation) to start running from.
    	*
    	* @todo Refer to documentation on date format.
    	*/
    explicit KStars( bool doSplash, bool startClockRunning = true, const QString &startDateString = QString() );

    /**Destructor.  Synchs config file.  Deletes objects.
    	*/
    ~KStars();

    /**@return pointer to KStarsData object which contains application data.
    	*/
    KStarsData* data();

    /**@return pointer to the local sidereal time.
    	*/
    dms* LST();

    /**@return pointer to SkyMap object which is the sky display widget.
    	*/
    SkyMap* map();

    ObservingList* observingList();

    ImageViewer* addImageViewer(const KUrl &url, const QString &message );
    void removeImageViewer( ImageViewer* );

    /**@return pointer to GeoLocation object which is the current geographic location.
    	*/
    GeoLocation* geo();

    /**@return pointer to InfoBoxes object.
    	*/
    InfoBoxes* infoBoxes();

    /**@return pointer to the INDI driver
    	*/
    INDIDriver* getINDIDriver() { return indidriver; }

    /**@return pointer to the INDI menu
    	*/
    INDIMenu* getINDIMenu() { return indimenu; }

    /** Establish the INDI system. No GUI
    	*/
    void establishINDI();

    /**Add an item to the color-scheme action manu
    	*@param name The name to use in the menu
    	*@param actionName The internal name for the action (derived from filename)
    	*/
    void addColorMenuItem( const QString &name, const QString &actionName );

    /**Remove an item from the color-scheme action manu
    	*@param actionName The internal name of the action (derived from filename)
    	*/
    void removeColorMenuItem( const QString &actionName );

    /**@short Apply config options throughout the program.
    	*In most cases, options are set in the "Options" object directly, 
    	*but for some things we have to manually react to config changes.
    	*/
    void applyConfig();

public Q_SLOTS:
    /**DCOP interface function.
    	*Set focus to given Ra/Dec coordinates 
    	*@param ra the Right Ascension coordinate for the focus (in Hours)
    	*@param dec the Declination coordinate for the focus (in Degrees)
    	*/
    Q_SCRIPTABLE Q_NOREPLY void setRaDec( double ra, double dec );

    /**DCOP interface function.
    	*Set focus to given Alt/Az coordinates. 
    	*@param alt the Altitude coordinate for the focus (in Degrees)
    	*@param az the Azimuth coordinate for the focus (in Degrees)
    	*/
    Q_SCRIPTABLE Q_NOREPLY void setAltAz(double alt, double az);

    /**DCOP interface function.
    	*Point in the direction described by the string argument.  
    	*@param direction either an object name, a compass direction (e.g., "north"), or "zenith"
    	*/
    Q_SCRIPTABLE Q_NOREPLY void lookTowards( const QString &direction );

    /**DCOP interface function.  Zoom in one step. */
    Q_SCRIPTABLE Q_NOREPLY void zoomIn() { slotZoomIn(); }

    /**DCOP interface function.  Zoom out one step. */
    Q_SCRIPTABLE Q_NOREPLY void zoomOut(){ slotZoomOut(); }

    /**DCOP interface function.  reset to the default zoom level. */
    Q_SCRIPTABLE Q_NOREPLY void defaultZoom() { slotDefaultZoom(); }

    /**DCOP interface function.  Set zoom level to specified value.
    	*@param z the zoom level.  Units are pixels per radian.
    	*/
    Q_SCRIPTABLE Q_NOREPLY void zoom(double z);

    /**DCOP interface function.  Set local time and date.
    	*@param yr year of date
    	*@param mth month of date
    	*@param day day of date
    	*@param hr hour of time
    	*@param min minute of time
    	*@param sec second of time
    	*/
    Q_SCRIPTABLE Q_NOREPLY void setLocalTime(int yr, int mth, int day, int hr, int min, int sec);

    /**DCOP interface function.  Delay further execution of DCOP commands.
    	*@param t number of seconds to delay
    	*/
    Q_SCRIPTABLE Q_NOREPLY void waitFor( double t );

    /**DCOP interface function.  Pause further DCOP execution until a key is pressed.
    	*@param k the key which will resume DCOP execution
    	*/
    Q_SCRIPTABLE Q_NOREPLY void waitForKey( const QString &k );

    /**DCOP interface function.  Toggle tracking.
    	*@param track engage tracking if true; else disengage tracking
    	*/
    Q_SCRIPTABLE Q_NOREPLY void setTracking( bool track );

    /**DCOP interface function.  modify a view option.
    	*@param option the name of the option to be modified
    	*@param value the option's new value
    	*/
    Q_SCRIPTABLE Q_NOREPLY void changeViewOption( const QString &option, const QString &value );

    /**DCOP interface function.
    	*@param name the name of the option to query
    	*@return the current value of the named option
    	*/
    QString getOption( const QString &name );

    /**DCOP interface function.  Read config file.
    	*This function is useful for restoring the user settings from the config file, 
    	*after having modified the settings in memory.
    	*@sa writeConfig()
    	*/
    Q_SCRIPTABLE Q_NOREPLY void readConfig();

    /**DCOP interface function.  Write current settings to config file.
    	*This function is useful for storing user settings before modifying them with a DCOP
    	*script.  The original settings can be restored with readConfig().
    	*@sa readConfig()
    	*/
    Q_SCRIPTABLE Q_NOREPLY void writeConfig();

    /**DCOP interface function.  Show text message in a popup window.
    	*@note Not Yet Implemented
    	*@param x x-coordinate for message window
    	*@param y y-coordinate for message window
    	*@param message the text to display in the message window
    	*/
    Q_SCRIPTABLE Q_NOREPLY void popupMessage( int x, int y, const QString &message );

    /**DCOP interface function.  Draw a line on the sky map.
    	*@note Not Yet Implemented
    	*@param x1 starting x-coordinate of line
    	*@param y1 starting y-coordinate of line
    	*@param x2 ending x-coordinate of line
    	*@param y2 ending y-coordinate of line
    	*@param speed speed at which line should appear from start to end points (in pixels per second)
    	*/
    Q_SCRIPTABLE Q_NOREPLY void drawLine( int x1, int y1, int x2, int y2, int speed );

    /**DCOP interface function.  Set the geographic location.
    	*@param city the city name of the location
    	*@param province the province name of the location
    	*@param country the country name of the location
    	*/
    Q_SCRIPTABLE Q_NOREPLY void setGeoLocation( const QString &city, const QString &province, const QString &country );

    /**DCOP interface function.  Modify a color.
    	*@param colorName the name of the color to be modified (e.g., "SkyColor")
    	*@param value the new color to use
    	*/
    Q_SCRIPTABLE Q_NOREPLY void setColor( const QString &colorName, const QString &value );

    /**DCOP interface function.  Load a color scheme.
    	*@param name the name of the color scheme to load (e.g., "Moonless Night")
    	*/
    Q_SCRIPTABLE Q_NOREPLY void loadColorScheme( const QString &name );

    /**DCOP interface function.  Export the sky image to a file.
    	*@param filename the filename for the exported image
    	*@param width the width for the exported image
    	*@param height the height for the exported image
    	*/
    Q_SCRIPTABLE Q_NOREPLY void exportImage( const QString &filename, int width, int height );

    /**DCOP interface function.  Print the sky image.
    	*@param usePrintDialog if true, the KDE print dialog will be shown; otherwise, default parameters will be used
    	*@param useChartColors if true, the "Star Chart" color scheme will be used for the printout, which will save ink.
    	*/
    Q_SCRIPTABLE Q_NOREPLY void printImage( bool usePrintDialog, bool useChartColors );

    // TODO INDI Scripting to be supported in KDE 4.1
	#if 0
    /**DCOP interface function.  Establish an INDI driver.
    	*@param deviceName The INDI device name
    	*@param useLocal establish driver locally?
    	*/
    Q_SCRIPTABLE Q_NOREPLY void startINDI (const QString &deviceName, bool useLocal);

    /**DCOP interface function. Set current device. All subsequent functions will
    	 communicate with this device until changed.
    	 *@param deviceName The INDI device name
    	*/
    Q_SCRIPTABLE Q_NOREPLY void setINDIDevice (const QString &deviceName);

    /**DCOP interface function. Shutdown an INDI driver.
    	*@param driverName the name of the driver to be shut down
    	*/
    Q_SCRIPTABLE Q_NOREPLY void shutdownINDI (const QString &driverName);

    /**DCOP interface function.  Turn INDI driver on/off.
    	*@param turnOn if true, turn driver on; otherwise turn off
    	*/
    Q_SCRIPTABLE Q_NOREPLY void switchINDI(bool turnOn);

    /**DCOP interface function.  Set INDI connection port.
    	*@param port the port identifier
    	*/
    Q_SCRIPTABLE Q_NOREPLY void setINDIPort(const QString &port);

    /**DCOP interface function.  Set INDI target RA/DEC coordinates
    	*@param RA the target's Right Ascension coordinate (in Hours) 
    	*@param DEC the target's Declination coordinate (in Degrees) 
    	*/
    Q_SCRIPTABLE Q_NOREPLY void setINDITargetCoord(double RA, double DEC);

    /**DCOP interface function.  Set INDI target to a named object.
    	*@param objectName the name of the object to be targeted
    	*/
    Q_SCRIPTABLE Q_NOREPLY void setINDITargetName(const QString &objectName);

    /**DCOP interface function.  Set INDI action.
    	*@param action the action to set
    	*/
    Q_SCRIPTABLE Q_NOREPLY void setINDIAction(const QString &action);

    /**DCOP interface function.  Pause DCOP execution until named INDI action is completed.
    	*@param action the action which is to be completed before resuming DCOP execution
    	*/
    Q_SCRIPTABLE Q_NOREPLY void waitForINDIAction(const QString &action);

    /**DCOP interface function.  Set INDI focus speed.
    	*@param speed the speed to use
    	*
    	* @todo document units for speed
    	*/
    Q_SCRIPTABLE Q_NOREPLY void setINDIFocusSpeed(unsigned int speed);

    /**DCOP interface function.  Set INDI focus direction and focus.
    	*@param focusDir 0 = focus in; 1 = focus out
    	*/
    Q_SCRIPTABLE Q_NOREPLY void startINDIFocus(int focusDir);

    /**DCOP interface function.  Set INDI geographical information.
    	*@param longitude the longitude to set, in Degrees
    	*@param latitude the latitude to set, in Degrees
    	*/
    Q_SCRIPTABLE Q_NOREPLY void setINDIGeoLocation(double longitude, double latitude);

    /**DCOP interface function.  Sets focus operation timeout.
    	*@param timeout the timeout interval, in seconds (?)
    	*/
    Q_SCRIPTABLE Q_NOREPLY void setINDIFocusTimeout(int timeout);

    /**DCOP interface function.  Start camera exposure with a timeout.
    	*@param timeout the exposure time, in seconds (?)
    	*/
    Q_SCRIPTABLE Q_NOREPLY void startINDIExposure(int timeout);

    /**DCOP interface function.  Set INDI UTC date and time.
    	*@param UTCDateTime the UTC date and time (e.g., "23 June 2004 12:30:00" ?)
    	*/
    Q_SCRIPTABLE Q_NOREPLY void setINDIUTC(const QString &UTCDateTime);

    /**DCOP interface function. Set INDI Telescope action.
    	*@param action the action to set
    	*/
    Q_SCRIPTABLE Q_NOREPLY void setINDIScopeAction(const QString &action);

    /**DCOP interface function. Set CCD camera frame type.
    	*@param type the frame type
    	*/
    Q_SCRIPTABLE Q_NOREPLY void setINDIFrameType(const QString &type);

    /**DCOP interface function. Set CCD filter.
    	*@param filter_num identifier of the CCD filter
    	*/
    Q_SCRIPTABLE Q_NOREPLY void setINDIFilterNum(int filter_num);

    /**DCOP interface function. Set CCD target temperature.
    	*@param temp the target CCD temperature (in Celsius ?)
    	*/
    Q_SCRIPTABLE Q_NOREPLY void setINDICCDTemp(int temp);

	#endif

    /**
    	*Update time-dependent data and (possibly) repaint the sky map.
    	*@param automaticDSTchange change DST status automatically?
    	*/
    void updateTime( const bool automaticDSTchange = true );

    /**
    	*Apply new settings and redraw skymap
    	*/
    void slotApplyConfigChanges();

    /**
    	*action slot: Configure toolbars
    	*/
    void slotConfigureToolbars();

    /**
    	*action slot: Generate toolbars with new user-specified configuration
    	*/
    void slotApplyToolbarConfig();

    /**
    	*action slot: Zoom in one step
    	*/
    void slotZoomIn();

    /**
    	*action slot: Zoom out one step
    	*/
    void slotZoomOut();

    /**
    	*action slot: Set the zoom level to its default value
    	*/
    void slotDefaultZoom();

    /**
    	*action slot: Allow user to specify a field-of-view angle for the display window in degrees, 
    	*and set the zoom level accordingly.
    	*/
    void slotSetZoom();
    /**
    	*action slot: Toggle whether kstars is tracking current position
    	*/
    void slotTrack();

    /**
    	*action slot: open dialog for selecting a new geographic location
    	*/
    void slotGeoLocator();

    /**Delete FindDialog because ObjNames list has changed in KStarsData due to
    	*reloading star data. So list in FindDialog must be new filled with current data.
    	*/
    void clearCachedFindDialog();

    /**
    	*Remove all trails which may have been added to solar system bodies
    	*/
    void slotClearAllTrails();

private slots:
    /**
    	*action slot: sync kstars clock to system time
    	*/
    void slotSetTimeToNow();

    /**
    	*action slot: open a dialog for setting the time and date
    	*/
    void slotSetTime();

    /**
    	*action slot: toggle whether kstars clock is running or not
    	*/
    void slotToggleTimer();

    /**
    	*action slot: open dialog for finding a named object
    	*/
    void slotFind();

    /**
     * action slot: open KStars setup wizard
     */
    void slotWizard();

    /**
     * action slot: open KNewStuff window to download extra data.
     */
    void slotDownload();

    /**
     * action slot: open KStars calculator to compute astronomical
     * ephemeris
     */

    void slotCalculator();

    /**
     * action slot: open KStars AAVSO Light Curve Generator
     */

    void slotLCGenerator();

    /**
    * action slot: open Elevation vs. Time tool
    */

    void slotAVT();

    /**
     * action slot: open What's up tonight dialog
     */
    void slotWUT();

    /**
     * action slot: open the glossary
     */
    void slotGlossary();

    /**
     * action slot: open ScriptBuilder dialog
     */
    void slotScriptBuilder();

    /**
     * action slot: open Solar system viewer
     */
    void slotSolarSystem();

    /**
     * action slot: open Jupiter Moons tool
     */
    void slotJMoonTool();

    /**
     * action slot: open Telescope wizard
     */
    void slotTelescopeWizard();

    /**
     * action slot: open Telescope wizard
     */
    void slotTelescopeProperties();

    /**
     * action slot: open Image Sequence dialog
     */
    void slotImageSequence();

    /**
    * action slot: open INDI driver panel
    */
    void slotINDIDriver();

    /**
    * action slot: open INDI control panel
    */
    void slotINDIPanel();

    /**
     * action slot: open INDI configuration dialog
     */
    void slotINDIConf();

    /**
    	*action slot: open dialog for setting the view options
    	*/
    void slotViewOps();

    /** finish setting up after the kstarsData has finished
     */
    void datainitFinished(bool worked);

    /**Open new KStars window. */
    void newWindow();

    /**Close KStars window. */
    void closeWindow();

    /** Open FITS image. */
    void slotOpenFITS();

    /**Action slot to save the sky image to a file.*/
    void slotExportImage();

    /**Action slot to select a DCOP script and run it.*/
    void slotRunScript();

    /**Action slot to print skymap. */
    void slotPrint();

    /**Action slot to show tip-of-the-day window. */
    void slotTipOfDay();

    /**Action slot to set focus coordinates manually (opens FocusDialog). */
    void slotManualFocus();

    /**Meta-slot to point the focus at special points (zenith, N, S, E, W).
    	*Uses the name of the Action which sent the Signal to identify the
    	*desired direction.  */
    void slotPointFocus();

    /**Meta-slot to set the color scheme according to the name of the
    	*Action which sent the activating signal.  */
    void slotColorScheme();

    /**Select the Target symbol (a.k.a. field-of-view indicator) */
    void slotTargetSymbol();

    /**Invoke the Field-of-View symbol editor window */
    void slotFOVEdit();

    /**Toggle between Equatorial and Ecliptic coordinte systems */
    void slotCoordSys();

    /**Set the map projection according to the menu selection */
    void slotMapProjection();

    /**Toggle display of the observing list tool*/
    void slotObsList();

    /**Meta-slot to handle display toggles for all of the viewtoolbar buttons.
    	*uses the name of the sender to identify the item to change.  */
    void slotViewToolBar();

    /**Meta-slot to handle toggling display of GUI elements (toolbars and infoboxes)
    	*uses name of the sender action to identify the widget to hide/show.  */
    void slotShowGUIItem( bool );

    /**Re-assign the input focus to the SkyMap widget.
    	*/
    void mapGetsFocus();

    /**Toggle to and from full screen mode */
    void slotFullScreen();

    /**Save data to config file before exiting.*/
    void slotAboutToQuit();

private:
    /**
    	*Initialize Menu bar, toolbars and all Actions.
    	*/
    void initActions();

    /**
    	*Initialize Field-of-View symbols and FOV submenu
    	*/
    void initFOV();

    /**
    	*Initialize Status bar.
    	*/
    void initStatusBar();

    /**
    	*Initialize focus position
    	*/
    void initFocus();

    /**
    	*Build the KStars main window
    	*/
    void buildGUI();

    KStarsData *kstarsData;
    KStarsSplash *splash;
    SkyMap *skymap;

    TimeStepBox *TimeStep;

    KActionMenu *colorActionMenu, *fovActionMenu;

    LCGenerator *AAVSODialog;
    FindDialog *findDialog;

    //FIXME: move to KStarsData
    ObservingList *obsList;
    AltVsTime *avt;
    WUTDialog *wut;
    ScriptBuilder *sb;
    PlanetViewer *pv;
    JMoonTool *jmt;
    QList<ImageViewer*> m_ImageViewerList;

    INDIMenu *indimenu;
    INDIDriver *indidriver;
    imagesequence *indiseq;  /* We need imgsequence here because it runs in batch mode */

    QActionGroup *projectionGroup, *fovGroup, *cschemeGroup;

    int idSpinBox;
    bool DialogIsObsolete;
    bool StartClockRunning;
    QString StartDateString;

    QPalette OriginalPalette, DarkPalette;

};

#endif
