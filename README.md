# bigsh0t

A collection of frei0r plugins for VR video, with support for [Shotcut](https://shotcut.org/). [GPL-2.0](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html), like [frei0r](https://github.com/dyne/frei0r).

## Building

The build script assumes that you have cloned [frei0r](https://github.com/dyne/frei0r) next to this repo, and that your directory structure looks something like this:

    bigsh0t/
        src/
           ...
        CMakeLists.txt
        README.md
    frei0r/
        include/
            frei0r.hpp

Bigsh0t will use OpenMP on Windows.

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
 * **fov**: The horizontal field of view, in degrees, of the resulting frame.
 * **yaw**, **pitch** and **roll**: The direction of the image center in the panorama.

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
