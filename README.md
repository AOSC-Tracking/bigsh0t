# bigsh0t

A collection of frei0r plugins for VR video, with support for [Shotcut](https://shotcut.org/). [GPL-2.0](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html), like [frei0r](https://github.com/dyne/frei0r).

## IMPORTANT NOTE

These plugins do not work with "Parallel processing" when exporting video. If you see tearing when using keyframes, disable parallel processing when exporting.

## Upgrade

If you are upgrading from bigsh0t 1.0, you need to remove the previous plugins and edit your `.mlt` files. Change:

    <property name="mlt_service">frei0r.hemi_to_eq</property>
    <property name="shotcut:filter">hemi_to_eq</property>

to:

    <property name="mlt_service">frei0r.bigsh0t_hemi_to_eq</property>
    <property name="shotcut:filter">bigsh0t_hemi_to_eq</property>


## Build

The build script comes with its own copy of the Frei0r headers.

### Windows 64 bit

Make sure that you compile to a 64-bit target. If you compile to 32-bit DLL:s, Shotcut will not be able to load them. Bigsh0t will use OpenMP on Windows.

### OSX

Shotcut will use OpenMP and requires it to be installed as when using `brew`:

    brew install libomp

## Install

There is no installer. Depending on the platform you are on, you'll have to copy the files into the right folders. When the build process is completed you should have a `.zip` or `.tar.gz` in the CMake build directory.

### Windows

Assuming Shotcut is installed in `C:\Program Files\Shotcut`, copy the DLLs into `C:\Program Files\Shotcut\lib\frei0r-1` and everything in the `shotcut/filters` folder into `C:\Program Files\Shotcut\share\shotcut\qml\filters`.

### OSX

Assuming Shotcut is installed in `/Applications/Shotcut.app`, copy the `.so` files into `/Applications/Shotcut.app/Contents/MacOS/lib/frei0r-1` and everything in the `shotcut/filters` folder into `/Applications/Shotcut.app/Contents/MacOS/share/shotcut/qml/filters`.

## Plugins

### Hemispherical to Equirectangular

Converts a video frame with two hemispherical images to a single equirectangular frame. The plugin assumes that both hemispheres are in the frame. If you have a camera like the Garmin Virb360 that produces two videos, one from each camera, you should start by converting them to a single movie by placing them side by side using, for example, [ffmpeg](https://www.ffmpeg.org/) (you can also add parameters to produce lossless, intra-only output here for easier editing):

    ffmpeg \
        -i left.mp4 \
        -i right.mp4 \
        -filter_complex hstack \
        output.mp4

#### Parameters

 * **interpolation**: Nearest-neighbor or bilinear. Determines the sampling method.
 * **projection**: The fisheye projection type. Currently only equidistant fisheyes, like the Ricoh Theta and Garmin Virb360 are supported.
 * **fov** and **radius**: The field of view of a single hemisphere in degrees, and the radius of the image circle, expressed as a fraction of the frame width.
 * **yaw**, **pitch** and **roll**: The two fisheye cameras of a 360 camera are rarely perfectly 180 degrees apart. These parameters adds a rotation to both lenses to correct for this.
 * **X**, **Y** and **Up**: The image center and image orientation for the front- and back-facing camera. The **X** parameter is a fraction of the image width, and the **Y** parameter a fraction of the image height. **Up** is the "up" direction in the image, in degrees clockwise from a direction towards the frame top edge.
 * **Nadir radius** and **Nadir start**: 360 cameras like the Theta have a problem with the nadir direction where, no matter what, you will have a little of the camera in the image. This parameter "stretches" the image near nadir to cover up the missing parts.

### Equirectangular to Rectilinear

Converts an equirectangular frame (panoramic) to a rectilinear frame (what you're used to seeing). Can be used to preview what will be shown in a 360 video viewer.

#### Parameters

 * **interpolation**: Nearest-neighbor or bilinear. Determines the sampling method.
 * **fov**: The horizontal field of view, in degrees, of the resulting frame. Any value over 179 results in a fisheye projection.
 * **yaw**, **pitch** and **roll**: The direction of the image center in the panorama.
 * **fisheye**: The amount of fisheye to mix in. 100 means that you get a 100% fisheye lens.

### Transform 360

Rotates a panoramic image.

#### Parameters

 * **interpolation**: Nearest-neighbor or bilinear. Determines the sampling method.
 * **yaw**, **pitch** and **roll**: The amount to rotate the image.

### Equirectangular Mask

Adds a black matte to the frame. Use this if you filmed using a 360 camera but only want to use part of the 360 image - for example if you and the film crew occupy the 90 degrees behind the camera.

#### Parameters

 * **HFOV Start** and **HFOV End**: The **Start** is the width in degrees of the un-matted area. The **End** is the width in degrees where the matte is at 100%.

 * **VFOV Start** and **VFOV End**: The **Start** is half the height in degrees of the un-matted area. The **End** is the half the height in degrees where the matte is at 100%.

### Rectilinear to Equirectangular

Converts a rectilinear (a normal-looking) image to an equirectangular image. Use this together with **Transform 360** to place "normal" footage in a 360 movie.

#### Parameters

 * **interpolation**: Nearest-neighbor or bilinear. Determines the sampling method.
 * **HFOV**: The width in degrees of the rectilinear image.
 * **VFOV**: The width in degrees of the rectilinear image.

### Stabilize 360

Stabilizes 360 footage. The plugin works in two phases - analysis and stabilization. When analyzing footage, it detects frame-to-frame rotation, and when stabilizing it tries to correct high-frequency motion (shake).

#### How to Stabilize Video

 1. Disable **Realtime (frame dropping)** in the **Settings** menu. It is important that the filter sees all frames.
 2. Add the 360 footage
 3. Apply a **Hemispherical to Equirectangular** filter to it so it is in equirectangular format.
 4. Apply a **Transform 360** filter
 5. Apply the **Stabilize 360** filter.
 6. Select a file to store stabilization data in.
 7. Enable **Analyze** mode.
 8. Use the **Transform 360** filter to rotate the footage so that the point straight ahead is over the center cluster of track points.
 9. Play the footage from start to finish. The FUD (Filter-Up Display) should show an increasing frame count.
 10. When the footage has completed playing, switch off the **Analyze** mode.
 11. You should now have stable 360 video.

#### The FUD (Filter-Up Display)

When you switch to analysis mode, the filter will overlay information on the video.

 * In the top left is information about the file you use for storage and how many frames it has data for.

 * The squares are the areas used for motion detection.

     * Red outline are the areas that were sampled.
     * Yellow outline is the search radius
     * Blue squares are fixed.
     * Green squares are where the sampled areas were found.

 * When analyzing, the red, yellow and green squares are drawn first. Then the frame is transformed in such a way as to cancel the rotation from the previous frame, and the blue squares are drawn. If the detection and detected rotation are flawless, the blue and green squares should overlap completely and form cyan squares.

 * The waveform drawn in the lower middle shows the detected motion. Red is yaw, green is pitch and blue is roll.


#### Parameters

 * **Mode**: Toggle this checkbox to go from stabilization mode to analysis mode.

 * **File**: Path to file that will be used to store the analysis data.

 * **Interpolation**: Output quality.

 * **Analysis: Sample Radius**: The radius of the square that the stabilizer will sample.

 * **Analysis: Search Radius**: The maximum amount of motion the stabilizer will detect.

 * **Analysis: Offset**: The distance between the track points.

 * **Analysis: Use backwards-facing track points**: If set, six backwards-facing track points will also be used to detect pitch and yaw motion. Disable if, for example, you show up holding the camera there.

 * **Yaw / Pitch / Roll: Amount**: The amount of stabilization to apply. 100% means that the stabilizer will make the camera as steady as it can. Smaller values reduce the amount of stabilization.

 * **Yaw / Pitch / Roll: Smoothing**: The number of frames to use to smooth out the shakes. The higher the value, the slower the camera will follow any intended motion.

 * **Yaw / Pitch / Roll: Time Bias**: Shift the frames used to smooth out the shakes relative to the stabilizaed frame. A value less than zero will give more weight to past frames, and the camera will seem to lag behind intended movement. A value greater than zero will give more weight to future frames, and the camera will appear to move ahead of the intended camera movement. A value of zero should make the camera follow the intended path.
