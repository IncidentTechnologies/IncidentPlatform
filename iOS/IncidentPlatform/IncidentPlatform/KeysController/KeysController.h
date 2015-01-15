/*!
 @header KeysController Library
 An iOS library to connect to the Keys MIDI device.
 @author Kate Schnippering
 @copyright Incident Technologies.
 @version 1.0.5
 @updated 2014-10-23
 @unsorted
 */

#import <Foundation/Foundation.h>
#import "Keys.h"

// Debug Defines
//#define KEYS_ALERT_LOG

#define WAIT_INT 0.25f
#define DEFAULT_KEY_MIN 48
#define DEFAULT_KEY_MAX 71

#define COLOR_RED_SHIFT 1.2
#define COLOR_GREEN_SHIFT 0.8
#define COLOR_BLUE_SHIFT 0.8

/*!
 @abstract The result status code used by the KeysController.
 @discussion These are the values that are returned by the KeysController. A status of 'KeysControllerStatusOk' means
 success, any other value is an error.
 @const KeysControllerStatusOk No errors occurred.
 @const KeysControllerStatusError An error occurred. Running with a 'logLevel' of 'KeysControllerLogLevelError' or
 greater will output additional details about the error.
 @const KeysControllerStatusNotConnected An error occurred because the Keys device was not connected. Try connecting
 to the device to resolve the error.
 @const KeysControllerStatusInvalidParameter An error occurred because of an invalid parameter. Running with a 'logLevel'
 of 'KeysControllerLogLevelError' or greater will output specific information about the invalid parameter.
 */
enum KeysControllerStatus
{
    KeysControllerStatusOk = 0,
    KeysControllerStatusError,
    KeysControllerStatusNotConnected,
    KeysControllerStatusInvalidParameter
};
typedef enum KeysControllerStatus KeysControllerStatus;

/*!
 @abstract The detail level of log output
 @discussion There is a variable amount of information that can be output to the standard output console (e.g. NSLog).
 These values define the different levels. Note that a given log level also includes all messages of a lower level, e.g.
 'KeysControllerLogLevelInfo' will also output all 'KeysControllerLogLevelWarn' and 'KeysControllerLogLevelError' messages.
 @const KeysControllerLogLevelOff Do not log any messages.
 @const KeysControllerLogLevelError Only log messages relating to errors (e.g. messages relating to 'KeysControllerStatusError').
 @const KeysControllerLogLevelWarn Log messages relating to non-critical errors.
 @const KeysControllerLogLevelInfo Log informational messages.
 @const KeysControllerLogLevelAll Log all messages.
 */
enum KeysControllerLogLevel
{
    KeysControllerLogLevelOff = 0,
    KeysControllerLogLevelError,
    KeysControllerLogLevelWarn,
    KeysControllerLogLevelInfo,
    KeysControllerLogLevelAll
};
typedef enum KeysControllerLogLevel KeysControllerLogLevel;

/*!
 @abstract The threads that 'KeysControllerObserver' selectors can run on.
 @discussion The KeysController communicates with the Keys device using the iOS CoreMIDI framework. The MIDI messages
 from the device are handled on a background thread, asynchronously to the main run loop. The KeysController can
 immediately notify observers directly from the MIDI background thread, or it can queue up notifications to run on the
 main thread when the current run loop (if any) completes. Although notifications from the MIDI thread are going to be
 delivered sooner, there may be concurrency issues associated with asynchronous delivery unless the receiving observer
 has been designed to handle such occurances.
 @const KeysControllerThreadMain Notify the observers on the main thread when the current run loop completes.
 @const KeysControllerThreadMidi Notify the observers immediately (asynchronously) from the MIDI background thread.
 Note that many iOS Frameworks (e.g. UIKit) will not work properly when running on anything other than the main thread.
 */
enum KeysControllerThread
{
    KeysControllerThreadMain = 0,
    KeysControllerThreadMidi
};
typedef enum KeysControllerThread KeysControllerThread;

/*!
 @abstract The effects available to the Keys device.
 @discussion There are several special effects available on the Keys device.
 @const KeysControllerEffectNone No special effects.
 @const KeysControllerEffectKeyFollow When the user presses a key down, the underlying LED is
 lit. The LED is turned off when the string and fret are unpressed.
 @const KeysControllerEffectNoteActive When a note is plucked, the underlying LED is lit.
 @const KeysControllerEffectKeyFollowNoteActive Both Key Follow and Note Active effects are active.
 */
enum KeysControllerEffect
{
    KeysControllerEffectNone = 0,
    KeysControllerEffectKeyFollow,
    KeysControllerEffectNoteActive,
    KeysControllerEffectKeyFollowNoteActive
};
typedef enum KeysControllerEffect KeysControllerEffect;

/*!
 @abstract A protocol for receiving events from the Keys device.
 @discussion An object may register itself with the KeysController as an observer. All registered observers will be
 notified via this protocol when the Keys device sends a message.
 @unsorted
 */
@protocol KeysControllerObserver <NSObject>
@optional

/*!
 @abstract Key pressed notification.
 @param position The position where the key was pressed.
 @discussion This function is called when a user presses down a key with their finger.
 */
- (void)keysDown:(KeyPosition)position;

/*!
 @abstract Key released notification.
 @param position The position where the key was released.
 @discussion This function is called when a user releases a key.
 */
- (void)keysUp:(KeyPosition)position;

/*!
 @abstract Note on notifications.
 @param pluck The pluck that occurred.
 @discussion This function is called when a user plucks a note. If a string is plucked while no frets have been pressed
 (the "open string"), the 'fret' value in KeysPosition will be 0. This is a normal and valid value.
 */
- (void)keysNoteOn:(KeysPress)press;

/*!
 @abstract Note off notification.
 @param position The position where the note off event occurred.
 @discussion This function is called when a note is turned off. This is analogous (on a real instrument) to a pressed note ending, due to decay over time, or interrupted with another press. The note off notification is sent in a variety of cases.
 */
- (void)keysNoteOff:(KeyPosition)position;

/*!
 @abstract Keys connected notification.
 @discussion This function is called when a Keys has been plugged in and the KeysController has successfully connected with
 it. If the Keys is already connected, any observer added via 'addObserver' will automatically get the connected notification.
 This makes it easy for applications to properly provide a gatekeeper screen: Initially display a message to the users to "Connect
 the Keys before proceeding", and then remove this screen when 'keysConnected' is called. You will be guaranteed to received
 the 'keysConnected' message when added as an observer, even if the the Keys has been connected previous (e.g. before the app
 even started up).
 */
- (void)keysConnected;

/*!
 @abstract Keys disconnected notification.
 @discussion This function is called when a Keys has been disconnected. Note that unlike 'keysConnected', 'keysDisconnected'
 is NOT called when 'removeObserver' removes an observer. This is because, in most cases, disconnecting a Keys while
 the app is running is effectively an error case, whereas 'removeObserver' is a very common occurance, so they most
 likely should be handled very differently.
 */
- (void)keysDisconnected;

/*!
 @abstract Keys range change notification
 @discussion This function is called when an additional Keys board has been added/removed.
 */
- (void)keysRangeChange:(KeysRange)range;

@end

/*!
 @class KeysController
 @abstract Keys interface library.
 @discussion A library to connect to the Keys device.
 @unsorted
 */
@interface KeysController : NSObject

/*!
 @abstract Returns a shared instance of the KeysController.
 @discussion A shared singleton instance of the KeysController that can be
 accessed globally via this class method.
 */
+ (KeysController*)sharedInstance;

/*!
 @abstract Information about the KeysController.
 @discussion Query this string for some basic properties about the version of the KeysController you are using.
 */
@property (nonatomic, readonly) NSString * info;

/*!
 @abstract Connection status of a keyboard device.
 @discussion Query the state of the keyboard device. When the keyboard device has been detected and gone through the handshake
 process, this propery is set to 'YES'. Changes to the connection state are simultaneously announced to observers via
 the 'keysConnected' and 'keysDisconnected' selectors.
 */
@property (nonatomic, readonly) BOOL connected;

/*!
 @abstract Connection status of the Keys device.
 @discussion Query the state of the Keys device. When the Keys device has been detected and gone through the handshake
 process, this propery is set to 'YES'. Changes to the connection state are simultaneously announced to observers via
 the 'keysConnected' and 'keysDisconnected' selectors.
 */
@property (nonatomic, readonly) BOOL isKeysDeviceConnected;

/*!
 @abstract Whether the KeysController connection is spoofed.
 @discussion For testing purposes, especially in the Simulator, the KeysController can pretend to be connected to a Keys
 device. This property is set to 'YES' when 'debugSpoofConnected' is called, and 'NO' when 'debugSpoofDisconnected' is called.
 */
@property (nonatomic, readonly) BOOL spoofed;

/*!
 @abstract Current range of the Keys device.
 @discussion When the Keys device has been connected, query the range. This property will be set when the 'keysRangeChange:' selector is announced to observers.
 */
@property (nonatomic, assign) KeysRange range;

/*!
 @abstract Set the thread that 'KeysControllerObserver' objects are notified on.
 @discussion The KeysController communicates with the Keys device using the iOS CoreMIDI framework. The MIDI messages
 from the device are handled on a background thread, asynchronously to the run loop on the main thread. The KeysController can
 immediately notify observers directly from the MIDI background thread, or it can queue up the notification to run on the Main
 thread when the current run loop (if any) completes. By default, 'responseThread' is set to 'KeysControllerThreadMain'
 and notifications are delivered on the main thead to minimize unintended concurrency issues. This may be changed to
 'KeysControllerThreadMidi' if faster notification time is desired, or if the main run loop is expected to run
 continously for long stretches of time and needs to be asynchronously interrupt.
 */
@property (nonatomic, assign) KeysControllerThread responseThread;

/*!
 @abstract The current level of detail output to the log.
 @discussion There is a variable amount of information that can be output to the standard output console (e.g. NSLog).
 The value assigned to this property sets the level of detail.
 */
@property (nonatomic, assign) KeysControllerLogLevel logLevel;

/*!
 @abstract The minimum interarrival time between Keys 'Note On' events.
 @discussion After a 'keysNoteOn' message has been delivered to the observers, the minimum interarrival time controls
 how much time must elapse before the next 'keysNoteOn' may be delivered. All string plucks on the Keys device before
 this time will be ignored. This can be useful, for example, to if an application does not need input faster than a
 specific tempo defines. It is measured in seconds.
 */
@property (nonatomic, assign) double minimumInterarrivalTime;

/*!
 @abstract The default color map for strings.
 @discussion A 'KeysLedColorMap' may be set to define default colors for each string. A 'KeysLedColor' corresponds to each
 string of the Keys device. When an LED is turned on with 'turnOnLedAtPositionWithColorMap', the KeysController will
 automatically use the 'KeysLedColor' for that string. */
@property (nonatomic, assign) KeysLedColorMap colorMap;

/*!
 @abstract Pretend to be connected to a Keys device.
 @result Returns KeysControllerStatusOk on success.
 @discussion This function puts the GuitarController into a spoofed 'connected' state, primarily for
 testing functionality in the Simulator. Successive calls to change Keys device state
 succeed (as no-ops). When connected, observers will receive the 'keysConnected' notification, as when
 a real Keys device is connected.
 */
- (KeysControllerStatus)debugSpoofConnected;

/*!
 @abstract Disconnect a spoofed Keys connection.
 @result Returns KeysControllerStatusOk on success.
 @discussion This function disconnects the GuitarController that is in a spoofed 'connected' state.
 */
- (KeysControllerStatus)debugSpoofDisconnected;

/*!
 @abstract Simulate a key range change on a spoofed connection.
 @result Return KeysControllerStatusOk on success. Returns new range?
 @discussion This function spoofs a Keys range change, observers will receive the 'keysRangeChange' notification.
 */
- (KeysControllerStatus)debugSpoofRangeChange:(NSTimer *)timer;

/*!
 @abstract Register to receive Keys device messages.
 @param observer An object that conforms to the KeysControllerObserver protocol. If 'object' is nil or already observing,
 a warning will be output, but the function will still return successfully.
 @result Returns KeysControllerStatusOk on success.
 @discussion The KeysController will notify all registered observers when the Keys device sends a message. Observers
 must implement the KeysControllerObserver protocol to receive notifications. The observer is not retained.
 */

- (KeysControllerStatus)addObserver:(id<KeysControllerObserver>)observer;
/*!
 @abstract Deregister to receive Keys device messages.
 @param observer An object that conforms to the KeysControllerObserver protocol. If 'object' is nil or already observing,
 a warning will be output, but the function will still return successfully.
 @result Returns KeysControllerStatusOk on success.
 @discussion The KeysController will stop notifying the observer of any future messages from the Keys device. The observer
 is not retained when added, and consquently will not be released when removed.
 */
- (KeysControllerStatus)removeObserver:(id<KeysControllerObserver>)observer;

/*!
 @abstract Turn off all LEDs.
 @result Returns KeysControllerStatusOk on success.
 @discussion Turn of all LEDs on the Keys device. This is identical to setting all LEDs brightness to zero.
 */
- (KeysControllerStatus)turnOffAllLeds;

/*!
 @abstract Turn off an LED at a position.
 @param position A fretboard position.
 @result Returns KeysControllerStatusOk on success.
 @discussion Turn off an LED at the given position. This is the same as calling 'turnOnLedAtPosition:withColor' with a
 color of (0,0,0). A value of 0 for either the fret or the string in the position acts as a "wild-card", applying to all
 LEDs along that string/fret. By extension, a position of (0,0) will turn off all LEDs on the Keys device.
 */
- (KeysControllerStatus)turnOffLedAtPosition:(KeyPosition)position;

/*!
 @abstract Turn on an LED at a given position with a given color.
 @param position A fretboard position.
 @param color An LED color.
 @result Returns KeysControllerStatusOk on success.
 @discussion Turn on an LED at a given position with a given color. A value of 0 for either the fret or the string in
 the position acts as a "wild-card", applying to all LEDs along that string/fret. By extension, a position of (0,0)
 will turn on all LEDs on the Keys device.
 */
- (KeysControllerStatus)turnOnLedAtPosition:(KeyPosition)position withColor:(KeysLedColor)color;

/*!
 @abstract Turn on an LED with the current color map.
 @param position A fretboard position.
 @result Returns KeysControllerStatusOk on success.
 @discussion The 'colorMap' property defines a default color for all LEDs along each string. This may be useful for applications that
 have a fixed color scheme for each string that doesn't change very much. After setting the 'colorMap', this function
 will turn on individual LEDs using the 'KeysLedColor' associated with the string. A value of 0 for either the fret or
 the string in the position acts as a "wild-card", applying to all LEDs along that string/fret. A position of (0,0)
 will turn on all LEDs on the Keys device, using each string's color mapping.
 */
- (KeysControllerStatus)turnOnLedAtPositionWithColorMap:(KeyPosition)position;

/*!
 @abstract Turn off all special effects.
 @result Returns KeysControllerStatusOk on success.
 @discussion Disable all of the special effects on the Keys device that have been previously enabled with 'enableEffect:withColor'
 */
- (KeysControllerStatus)turnOffAllEffects;

/*!
 @abstract Turn on special effects.
 @param effect The type of special effect.
 @param color An LED color.
 @result Returns KeysControllerStatusOk on success.
 @discussion Turn on special effects on the Keys device with a given color.
 */
- (KeysControllerStatus)turnOnEffect:(KeysControllerEffect)effect withColor:(KeysLedColor)color;

@end