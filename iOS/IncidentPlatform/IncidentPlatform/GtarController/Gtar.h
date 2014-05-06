/*!
 @header Gtar Header
    Common constants and macros used througout the Gtar SDK
 @author Marty Greenia
 @copyright Incident Technologies.
 @version 1.0.5
 @updated 2012-09-28
 @unsorted
 */

/*!
 @abstract The number of strings.
 @discussion The number of strings on the Gtar.
 */
static const NSInteger GtarStringCount = 6;

/*!
 @abstract The number of frets.
 @discussion The number of frets on the Gtar.
 */
static const NSInteger GtarFretCount = 16;

/*!
 @abstract The total number of LEDs on the Gtar.
 @discussion The total number of LEDs on the Gtar, equal to the number of strings times the number of frets.
 */
static const NSInteger GtarLedCount = 96;

/*!
 @abstract The maximum LED intensity.
 @discussion Each color component of each LED has multiple discrete intensity levels. Each intensity varies from [0..GtarMaxLedIntensity],
 where 0 is off and 'GtarMaxLedIntensity' is full brightness.
 */
static const NSInteger GtarMaxLedIntensity = 3;

/*!
 @abstract The maximum pluck velocity.
 @discussion When a string is plucked, its velocity is given a number from [0..GtarMaxPluckVelocity],
 where 0 is off and 'GtarMaxPluckVelocity' is full speed.
 */
static const NSInteger GtarMaxPluckVelocity = 127;

/*!
 @abstract A guitar string.
 @discussion A data type representing a string on the Gtar. The range of string values are [1..GtarStringCount], 
 corresponding to the strings of the Gtar device.
 */
typedef unsigned char GtarString;

/*!
 @abstract A guitar fret.
 @discussion A data type representing a fret on the Gtar. The range of fret values are [1..GtarFretCount], corresponding
 to the frets of the Gtar device. A GtarFret value of 0 is a special value for the "open string", when a string is plucked
 without any fret being pressed. This is a common and valid value.
 */
typedef unsigned char GtarFret;

/*!
 @abstract An LED intensity value.
 @discussion A data type representing the intensity of a single LED color on the Gtar. Acceptable values are in the range [0..GtarMaxLedIntensity].
 */
typedef unsigned char GtarLedIntensity;

/*!
 @abstract A pluck velocity.
 @discussion A data type representing the velocity of a guitar pluck. Acceptable values are in the range [0..GtarMaxPluckVelocity].
 */
typedef unsigned char GtarPluckVelocity;

/*!
 @abstract A position on the fretboard.
 @discussion A GtarPosition is a string-fret pair, often used as a representation for a plucked note from the Gtar. The range
 of 'fret' and 'string' is [1..GtarFretCount] and [1..GtarStringCount] respectively. A 'fret' value of
 0 is a special value for the 'open-string', when a string is plucked without any fret being pressed.
 @field fret The GtarFret for this note.
 @field string The GtarString for this note.
 */
typedef struct
{
    GtarFret fret;
    GtarString string;
} GtarPosition;

/*!
 @abstract A pluck on the guitar.
 @discussion A GtarPluck combines a GtarPosition (string-fret pair) with a GtarVelocity corresponding to the velocity
 of the pluck.
 @field position The GtarPosition of this pluck.
 @field velocity The GtarPluckVelocity of this pluck.
 */
typedef struct
{
    GtarPosition position;
    GtarPluckVelocity velocity;
} GtarPluck;

/*!
 @abstract The RGB color components of a guitar LED.
 @discussion A single Gtar LED is composed of three different color components. Different colors can be created by
 combining RGB components using standard color compositions for additive color. Each intensity varies from [0..GtarMaxLedIntensity],
 where 0 is off and 'GtarMaxLedIntensity' is full brightness. A GtarLedColor of (0,0,0) turns off an LED.
 
 Common colors include:
 
 Red = (GtarMaxLedIntensity, 0, 0)
 
 Green = (0, GtarMaxLedIntensity, 0)
 
 Blue = (0, 0, GtarMaxLedIntensity)
 
 White = (GtarMaxLedIntensity, GtarMaxLedIntensity, GtarMaxLedIntensity)
 
 Yellow = (GtarMaxLedIntensity, GtarMaxLedIntensity, 0)
 
 Orange = (GtarMaxLedIntensity, GtarMaxLedIntensity/2, 0)
 
 Purple = (GtarMaxLedIntensity/2, 0, GtarMaxLedIntensity/2)
 
 Pink = (GtarMaxLedIntensity, 0, GtarMaxLedIntensity)
 
 "Black" / Off = (0, 0, 0)
 
 @field red The GtarLedIntensity for the red channel.
 @field green The GtarLedIntensity for the green channel.
 @field blue The GtarLedIntensity for the blue channel.
 */
typedef struct
{
    GtarLedIntensity red;
    GtarLedIntensity green;
    GtarLedIntensity blue;
} GtarLedColor;

/*!
 @abstract A string-to-GtarLedColor color mapping.
 @discussion Define a per-string default color mapping. Each GtarLedColor in the 'stringColor' array maps to a string of
 the Gtar device. Note that 'stringColor' is a 0-based, C-style array, while GtarStrings are 1-based. So 'stringColor' ranges 
 from [0..GtarStringCount-1] and maps to string [1..GtarStringCount].
 @field stringColor Each string of the Gtar device has a GtarLedColor intensity associated with it.
 */
typedef struct
{
    GtarLedColor stringColor[GtarStringCount];
} GtarLedColorMap;

/*!
 @abstract Create a GtarLedColor.
 @result Returns GtarLedColor.
 @discussion Convenience method for creating GtarLedColor structures.
 */
static inline GtarLedColor GtarLedColorMake(GtarLedIntensity red, GtarLedIntensity green, GtarLedIntensity blue)
{
    GtarLedColor c; c.red = red; c.green = green; c.blue = blue; return c;
}

/*!
 @abstract Create a GtarPosition.
 @result Returns GtarPosition.
 @discussion Convenience method for creating GtarPosition structures.
 */
static inline GtarPosition GtarPositionMake(GtarFret fret, GtarString string)
{
    GtarPosition p; p.fret = fret; p.string = string; return p;
}
