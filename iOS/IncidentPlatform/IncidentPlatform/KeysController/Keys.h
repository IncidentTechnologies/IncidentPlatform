/*!
 @header Keys Header
 Common constants and macros used througout the Keys SDK
 @author Kate Schnippering
 @copyright Incident Technologies.
 @version 1.0.5
 @updated 2014-10-23
 @unsorted
 */

/*!
 @abstract The number of possible keys (by midi value range).
 @discussion This will actually be limited to 120, the last half of the octave is unattainable.
 */
static const NSInteger KeysMaxKeyPosition = 127;

/*!
 @abstract The total number of keys (notes) per octave.
 @discussion This number is used for coloring or other mod math.
 */
static const NSInteger KeysPerOctaveCount = 12;

/*!
 @abstract The total number of LEDs on the Keys.
 @discussion The total number of LEDs on the Keys, equal to the number of Keys on the board.
 */
static const NSInteger KeysLedCount = 24;

/*!
 @abstract The maximum LED intensity.
 @discussion Each color component of each LED has multiple discrete intensity levels. Each intensity varies from [0..KeysMaxLedIntensity],
 where 0 is off and 'KeysMaxLedIntensity' is full brightness.
 */
static const NSInteger KeysMaxLedIntensity = 3;

/*!
 @abstract The maximum press velocity.
 @discussion When a key is pressed, its velocity is given a number from [0..KeysMaxPressVelocity],
 where 0 is off and 'KeysMaxPressVelocity' is full speed.
 */
static const NSInteger KeysMaxPressVelocity = 127;

/*!
 @abstract A keyboard key.
 @discussion A data type representing a key on a Keys board in the range of [0..KeysMaxKeyPosition]. This is a MIDI value, the hardware will automatically convert any transposition and chaining.
 */
typedef unsigned char KeyPosition;

/*!
 @abstract An LED intensity value.
 @discussion A data type representing the intensity of a single LED color on the Keys. Acceptable values are in the range [0..KeysMaxLedIntensity].
 */
typedef unsigned char KeysLedIntensity;

/*!
 @abstract A press velocity.
 @discussion A data type representing the velocity of a key pressed. Acceptable values are in the range [0..KeysMaxPressVelocity].
 */
typedef unsigned char KeysPressVelocity;

/*!
 @abstract A press of a key.
 @discussion A KeysPress combines a KeysPosition (string-fret pair) with a KeysVelocity corresponding to the velocity
 of the pluck.
 @field position The KeysPosition of this pluck.
 @field velocity The KeysPressVelocity of this pluck.
 */
typedef struct
{
    KeyPosition position;
    KeysPressVelocity velocity;
} KeysPress;

/*!
 @abstract The RGB color components of a guitar LED.
 @discussion A single Keys LED is composed of three different color components. Different colors can be created by
 combining RGB components using standard color compositions for additive color. Each intensity varies from [0..KeysMaxLedIntensity],
 where 0 is off and 'KeysMaxLedIntensity' is full brightness. A KeysLedColor of (0,0,0) turns off an LED.
 
 Common colors include:
 
 Red = (KeysMaxLedIntensity, 0, 0)
 
 Green = (0, KeysMaxLedIntensity, 0)
 
 Blue = (0, 0, KeysMaxLedIntensity)
 
 White = (KeysMaxLedIntensity, KeysMaxLedIntensity, KeysMaxLedIntensity)
 
 Yellow = (KeysMaxLedIntensity, KeysMaxLedIntensity, 0)
 
 Orange = (KeysMaxLedIntensity, KeysMaxLedIntensity/2, 0)
 
 Purple = (KeysMaxLedIntensity/2, 0, KeysMaxLedIntensity/2)
 
 Pink = (KeysMaxLedIntensity, 0, KeysMaxLedIntensity)
 
 "Black" / Off = (0, 0, 0)
 
 @field red The KeysLedIntensity for the red channel.
 @field green The KeysLedIntensity for the green channel.
 @field blue The KeysLedIntensity for the blue channel.
 */
typedef struct
{
    KeysLedIntensity red;
    KeysLedIntensity green;
    KeysLedIntensity blue;
} KeysLedColor;

/*!
 @abstract A string-to-KeysLedColor color mapping.
 @discussion Define a per-string default color mapping. Each KeysLedColor in the 'keyColor' array maps to a key of the Keys device. Note that 'keyColor' is a 0-based, C-style array, while KeysStrings are 1-based. So 'keyColor' ranges from [0..KeysPerOctaveCount-1] and maps to string [1..KeysPerOctaveCount].
 @field keyColor Each string of the Keys device has a KeysLedColor intensity associated with it.
 */
typedef struct
{
    KeysLedColor keyColor[KeysPerOctaveCount];
} KeysLedColorMap;

/*!
 @abstract Create a KeysLedColor.
 @result Returns KeysLedColor.
 @discussion Convenience method for creating KeysLedColor structures.
 */
static inline KeysLedColor KeysLedColorMake(KeysLedIntensity red, KeysLedIntensity green, KeysLedIntensity blue)
{
    KeysLedColor c; c.red = red; c.green = green; c.blue = blue; return c;
}

