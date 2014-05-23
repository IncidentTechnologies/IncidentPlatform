/*!
 @header GtarController Library
    An iOS library to connect to the Gtar MIDI device.
 @author Marty Greenia
 @copyright Incident Technologies.
 @version 1.0.5
 @updated 2012-09-28
 @unsorted
 */

#import <Foundation/Foundation.h>
#import "Gtar.h"

// Debug Defines
//#define GTAR_ALERT_LOG

#define WAIT_INT 0.25f

/*!
 @abstract The result status code used by the GtarController.
 @discussion These are the values that are returned by the GtarController. A status of 'GtarControllerStatusOk' means 
  success, any other value is an error.
 @const GtarControllerStatusOk No errors occurred.
 @const GtarControllerStatusError An error occurred. Running with a 'logLevel' of 'GtarControllerLogLevelError' or
  greater will output additional details about the error.
 @const GtarControllerStatusNotConnected An error occurred because the Gtar device was not connected. Try connecting
  to the device to resolve the error.
 @const GtarControllerStatusInvalidParameter An error occurred because of an invalid parameter. Running with a 'logLevel'
  of 'GtarControllerLogLevelError' or greater will output specific information about the invalid parameter.
 */
enum GtarControllerStatus
{
    GtarControllerStatusOk = 0,
    GtarControllerStatusError,
    GtarControllerStatusNotConnected,
    GtarControllerStatusInvalidParameter
};
typedef enum GtarControllerStatus GtarControllerStatus;

/*!
 @abstract The detail level of log output
 @discussion There is a variable amount of information that can be output to the standard output console (e.g. NSLog). 
    These values define the different levels. Note that a given log level also includes all messages of a lower level, e.g. 
    'GtarControllerLogLevelInfo' will also output all 'GtarControllerLogLevelWarn' and 'GtarControllerLogLevelError' messages.
 @const GtarControllerLogLevelOff Do not log any messages.
 @const GtarControllerLogLevelError Only log messages relating to errors (e.g. messages relating to 'GtarControllerStatusError').
 @const GtarControllerLogLevelWarn Log messages relating to non-critical errors.
 @const GtarControllerLogLevelInfo Log informational messages.
 @const GtarControllerLogLevelAll Log all messages.
 */
enum GtarControllerLogLevel
{
    GtarControllerLogLevelOff = 0,
    GtarControllerLogLevelError,
    GtarControllerLogLevelWarn,
    GtarControllerLogLevelInfo,
    GtarControllerLogLevelAll
};
typedef enum GtarControllerLogLevel GtarControllerLogLevel;

/*!
 @abstract The threads that 'GtarControllerObserver' selectors can run on.
 @discussion The GtarController communicates with the Gtar device using the iOS CoreMIDI framework. The MIDI messages 
    from the device are handled on a background thread, asynchronously to the main run loop. The GtarController can 
    immediately notify observers directly from the MIDI background thread, or it can queue up notifications to run on the
    main thread when the current run loop (if any) completes. Although notifications from the MIDI thread are going to be
    delivered sooner, there may be concurrency issues associated with asynchronous delivery unless the receiving observer
    has been designed to handle such occurances.
 @const GtarControllerThreadMain Notify the observers on the main thread when the current run loop completes.
 @const GtarControllerThreadMidi Notify the observers immediately (asynchronously) from the MIDI background thread. 
    Note that many iOS Frameworks (e.g. UIKit) will not work properly when running on anything other than the main thread.
 */
enum GtarControllerThread
{
    GtarControllerThreadMain = 0,
    GtarControllerThreadMidi
};
typedef enum GtarControllerThread GtarControllerThread;

/*!
 @abstract The effects available to the Gtar device.
 @discussion There are several special effects available on the Gtar device.
 @const GtarControllerEffectNone No special effects.
 @const GtarControllerEffectFretFollow When the user presses a string into contact with a fret, the underlying LED is 
  lit. The LED is turned off when the string and fret are unpressed.
 @const GtarControllerEffectNoteActive When a note is plucked, the underlying LED is lit.
 @const GtarControllerEffectFretFollowNoteActive Both Fret Follow and Note Active effects are active.
 */
enum GtarControllerEffect
{
	GtarControllerEffectNone = 0,
	GtarControllerEffectFretFollow,
	GtarControllerEffectNoteActive,
	GtarControllerEffectFretFollowNoteActive
};
typedef enum GtarControllerEffect GtarControllerEffect;

/*!
 @abstract A protocol for receiving events from the Gtar device.
 @discussion An object may register itself with the GtarController as an observer. All registered observers will be 
    notified via this protocol when the Gtar device sends a message.
 @unsorted
 */
@protocol GtarControllerObserver <NSObject>
@optional

/*!
 @abstract Fret pressed notification.
 @param position The position where the fret was pressed.
 @discussion This function is called when a user presses a string to a fret with their finger. 
 */
- (void)gtarFretDown:(GtarPosition)position;

/*!
 @abstract Fret released notification.
 @param position The position where the fret was released.
 @discussion This function is called when a user releases a string from a fret.
 */
- (void)gtarFretUp:(GtarPosition)position;

/*!
 @abstract Note on notifications.
 @param pluck The pluck that occurred.
 @discussion This function is called when a user plucks a note. If a string is plucked while no frets have been pressed 
    (the "open string"), the 'fret' value in GtarPosition will be 0. This is a normal and valid value.
 */
- (void)gtarNoteOn:(GtarPluck)pluck;

/*!
 @abstract Note off notification.
 @param position The position where the note off event occurred.
 @discussion This function is called when a note is turned off. This is analogous (on a real instrument) to a plucked note
    ending, due to decay over time, another note being played on the same string, etc. The note off notification is sent 
    in a variety of cases.
 */
- (void)gtarNoteOff:(GtarPosition)position;

/*!
 @abstract Gtar connected notification.
 @discussion This function is called when a Gtar has been plugged in and the GtarController has successfully connected with
    it. If the Gtar is already connected, any observer added via 'addObserver' will automatically get the connected notification. 
    This makes it easy for applications to properly provide a gatekeeper screen: Initially display a message to the users to "Connect
    the Gtar before proceeding", and then remove this screen when 'gtarConnected' is called. You will be guaranteed to received
    the 'gtarConnected' message when added as an observer, even if the the Gtar has been connected previous (e.g. before the app
    even started up).
 */
- (void)gtarConnected;

/*!
 @abstract Gtar disconnected notification.
 @discussion This function is called when a Gtar has been disconnected. Note that unlike 'gtarConnected', 'gtarDisconnected'
    is NOT called when 'removeObserver' removes an observer. This is because, in most cases, disconnecting a Gtar while
    the app is running is effectively an error case, whereas 'removeObserver' is a very common occurance, so they most
    likely should be handled very differently.
 */
- (void)gtarDisconnected;
@end

/*!
 @class GtarController
 @abstract Gtar interface library.
 @discussion A library to connect to the Gtar device. 
 @unsorted
 */
@interface GtarController : NSObject

/*!
 @abstract Returns a shared instance of the GtarController.
 @discussion A shared singleton instance of the GtarController that can be
    accessed globally via this class method.
 */
+ (GtarController*)sharedInstance;

/*!
 @abstract Information about the GtarController.
 @discussion Query this string for some basic properties about the version of the GtarController you are using.
 */
@property (nonatomic, readonly) NSString * info;

/*!
 @abstract Connection status of the Gtar device.
 @discussion Query the state of the Gtar device. When the Gtar device has been detected and gone through the handshake 
  process, this propery is set to 'YES'. Changes to the connection state are simultaneously announced to observers via 
  the 'gtarConnected' and 'gtarDisconnected' selectors.
 */
@property (nonatomic, readonly) BOOL connected;

/*!
 @abstract Whether the GtarController connection is spoofed.
 @discussion For testing purposes, especially in the Simulator, the GtarController can pretend to be connected to a Gtar
  device. This property is set to 'YES' when 'debugSpoofConnected' is called, and 'NO' when 'debugSpoofDisconnected' is called.
 */
@property (nonatomic, readonly) BOOL spoofed;

/*!
 @abstract Set the thread that 'GtarControllerObserver' objects are notified on.
 @discussion The GtarController communicates with the Gtar device using the iOS CoreMIDI framework. The MIDI messages 
    from the device are handled on a background thread, asynchronously to the run loop on the main thread. The GtarController can 
    immediately notify observers directly from the MIDI background thread, or it can queue up the notification to run on the Main 
    thread when the current run loop (if any) completes. By default, 'responseThread' is set to 'GtarControllerThreadMain'
    and notifications are delivered on the main thead to minimize unintended concurrency issues. This may be changed to 
    'GtarControllerThreadMidi' if faster notification time is desired, or if the main run loop is expected to run 
    continously for long stretches of time and needs to be asynchronously interrupt.
 */
@property (nonatomic, assign) GtarControllerThread responseThread;

/*!
 @abstract The current level of detail output to the log.
 @discussion There is a variable amount of information that can be output to the standard output console (e.g. NSLog). 
  The value assigned to this property sets the level of detail.
 */
@property (nonatomic, assign) GtarControllerLogLevel logLevel;

/*!
 @abstract The minimum interarrival time between Gtar 'Note On' events.
 @discussion After a 'gtarNoteOn' message has been delivered to the observers, the minimum interarrival time controls
  how much time must elapse before the next 'gtarNoteOn' may be delivered. All string plucks on the Gtar device before
  this time will be ignored. This can be useful, for example, to if an application does not need input faster than a
  specific tempo defines. It is measured in seconds. 
 */
@property (nonatomic, assign) double minimumInterarrivalTime;

/*!
 @abstract The default color map for strings.
 @discussion A 'GtarLedColorMap' may be set to define default colors for each string. A 'GtarLedColor' corresponds to each
 string of the Gtar device. When an LED is turned on with 'turnOnLedAtPositionWithColorMap', the GtarController will
 automatically use the 'GtarLedColor' for that string. */
@property (nonatomic, assign) GtarLedColorMap colorMap;

/*!
 @abstract Pretend to be connected to a Gtar device.
 @result Returns GtarControllerStatusOk on success.
 @discussion This function puts the GuitarController into a spoofed 'connected' state, primarily for
    testing functionality in the Simulator. Successive calls to change Gtar device state 
    succeed (as no-ops). When connected, observers will receive the 'gtarConnected' notification, as when
    a real Gtar device is connected.
 */
- (GtarControllerStatus)debugSpoofConnected;

/*!
 @abstract Disconnect a spoofed Gtar connection.
 @result Returns GtarControllerStatusOk on success.
 @discussion This function disconnects the GuitarController that is in a spoofed 'connected' state.
 */
- (GtarControllerStatus)debugSpoofDisconnected;

/*!
 @abstract Register to receive Gtar device messages.
 @param observer An object that conforms to the GtarControllerObserver protocol. If 'object' is nil or already observing,
  a warning will be output, but the function will still return successfully.
 @result Returns GtarControllerStatusOk on success.
 @discussion The GtarController will notify all registered observers when the Gtar device sends a message. Observers 
  must implement the GtarControllerObserver protocol to receive notifications. The observer is not retained.
 */

- (GtarControllerStatus)addObserver:(id<GtarControllerObserver>)observer;
/*!
 @abstract Deregister to receive Gtar device messages.
 @param observer An object that conforms to the GtarControllerObserver protocol. If 'object' is nil or already observing,
  a warning will be output, but the function will still return successfully.
 @result Returns GtarControllerStatusOk on success.
 @discussion The GtarController will stop notifying the observer of any future messages from the Gtar device. The observer 
    is not retained when added, and consquently will not be released when removed.
 */
- (GtarControllerStatus)removeObserver:(id<GtarControllerObserver>)observer;

/*!
 @abstract Turn off all LEDs.
 @result Returns GtarControllerStatusOk on success.
 @discussion Turn of all LEDs on the Gtar device. This is identical to setting all LEDs brightness to zero.
 */
- (GtarControllerStatus)turnOffAllLeds;

/*!
 @abstract Turn off an LED at a position.
 @param position A fretboard position.
 @result Returns GtarControllerStatusOk on success.
 @discussion Turn off an LED at the given position. This is the same as calling 'turnOnLedAtPosition:withColor' with a 
    color of (0,0,0). A value of 0 for either the fret or the string in the position acts as a "wild-card", applying to all
    LEDs along that string/fret. By extension, a position of (0,0) will turn off all LEDs on the Gtar device.
 */
- (GtarControllerStatus)turnOffLedAtPosition:(GtarPosition)position;

/*!
 @abstract Turn on an LED at a given position with a given color.
 @param position A fretboard position.
 @param color An LED color.
 @result Returns GtarControllerStatusOk on success.
 @discussion Turn on an LED at a given position with a given color. A value of 0 for either the fret or the string in
    the position acts as a "wild-card", applying to all LEDs along that string/fret. By extension, a position of (0,0)
    will turn on all LEDs on the Gtar device.
 */
- (GtarControllerStatus)turnOnLedAtPosition:(GtarPosition)position withColor:(GtarLedColor)color;

/*!
 @abstract Turn on an LED with the current color map. 
 @param position A fretboard position.
 @result Returns GtarControllerStatusOk on success.
 @discussion The 'colorMap' property defines a default color for all LEDs along each string. This may be useful for applications that
    have a fixed color scheme for each string that doesn't change very much. After setting the 'colorMap', this function
    will turn on individual LEDs using the 'GtarLedColor' associated with the string. A value of 0 for either the fret or
    the string in the position acts as a "wild-card", applying to all LEDs along that string/fret. A position of (0,0) 
    will turn on all LEDs on the Gtar device, using each string's color mapping.
*/ 
- (GtarControllerStatus)turnOnLedAtPositionWithColorMap:(GtarPosition)position;

/*!
 @abstract Turn off all special effects.
 @result Returns GtarControllerStatusOk on success.
 @discussion Disable all of the special effects on the Gtar device that have been previously enabled with 'enableEffect:withColor'
 */
- (GtarControllerStatus)turnOffAllEffects;

/*!
 @abstract Turn on special effects.
 @param effect The type of special effect.
 @param color An LED color.
 @result Returns GtarControllerStatusOk on success.
 @discussion Turn on special effects on the Gtar device with a given color.
 */
- (GtarControllerStatus)turnOnEffect:(GtarControllerEffect)effect withColor:(GtarLedColor)color;

@end