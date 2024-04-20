import sys
import subprocess
import os
import tempfile
import time

def main() -> None:
    input = sys.argv[1]
    output_motion = sys.argv[2]
    output_movie = sys.argv[3] if len(sys.argv) > 3 else None
    source_xml = """
<?xml version="1.0" standalone="no"?>
<mlt LC_NUMERIC="C" version="7.23.0" title="Shotcut version 24.02.29">
  <chain id="chain0" title="Shotcut version 24.02.29">
    <property name="resource">{input}</property>
    <filter id="filter0">
      <property name="version">2.7</property>
      <property name="mlt_service">frei0r.bigsh0t_zenith_correction</property>
      <property name="shotcut:filter">bigsh0t_zenith_correction</property>
      <property name="interpolation">1</property>
      <property name="analysisFile">{input}</property>
      <property name="enableSmoothYaw">1</property>
      <property name="smoothYaw">120</property>
      <property name="timeBiasYaw">0</property>
      <property name="clipOffset">0</property>
    </filter>
    <filter id="filter1">
      <property name="version">2.7</property>
      <property name="mlt_service">frei0r.bigsh0t_transform_360</property>
      <property name="shotcut:filter">bigsh0t_transform_360</property>
      <property name="yaw">0</property>
      <property name="pitch">0</property>
      <property name="roll">0</property>
      <property name="interpolation">1</property>
      <property name="grid">0</property>
    </filter>
    <filter id="filter2">
      <property name="version">2.7</property>
      <property name="mlt_service">frei0r.bigsh0t_stabilize_360_v2</property>
      <property name="shotcut:filter">bigsh0t_stabilize_360_v2</property>
      <property name="analyze">1</property>
      <property name="filterUpDisplay">0</property>
      <property name="interpolation">1</property>
      <property name="subpixels">2</property>
      <property name="maxStep">16</property>
      <property name="analysisFile">{output_motion}</property>
      <property name="stabilizeYaw">100</property>
      <property name="stabilizePitch">100</property>
      <property name="stabilizeRoll">100</property>
      <property name="smoothYaw">120</property>
      <property name="smoothPitch">120</property>
      <property name="smoothRoll">120</property>
      <property name="timeBiasYaw">0</property>
      <property name="timeBiasPitch">0</property>
      <property name="timeBiasRoll">0</property>
      <property name="clipOffset">0</property>
    </filter>
  </chain>
</mlt>
    """.format(
        input=input,
        output_motion=output_motion
    ).strip()

    temp_xml_path = None
    with tempfile.NamedTemporaryFile(suffix=".mlt") as tmpfile:
        temp_xml_path = tmpfile.name

    delete_output_movie = False
    if output_movie is None:
        delete_output_movie = True
        with tempfile.NamedTemporaryFile(suffix=".mp4") as tmpfile:
            output_movie = tmpfile.name

    try:
        with open(temp_xml_path, "wb") as f:
            f.write(source_xml.encode("utf-8"))

        shotcut_home = "C:\\Program Files\\Shotcut"

        compiled_xml = subprocess.check_output(
            [
                os.path.join(shotcut_home, "melt.exe"),
                "-quiet", "-silent",
                temp_xml_path,
                "-consumer", "xml"
            ]
        ).decode("utf-8")

        consumer = """
        <consumer ab="384k" no_audio="1" no_video="1" acodec="aac" ar="48000" bf="3" channels="2" crf="23" deinterlacer="bwdif" f="mp4" g="150" mlt_service="avformat" movflags="+faststart" preset="fast" real_time="0" rescale="bilinear" target="{output_movie}" threads="0" top_field_first="2" vbr="off" vcodec="libx264"/>
        """.format(
            output_movie=output_movie
        )

        compiled_xml = compiled_xml.replace("<chain ", consumer + "\n<chain ")

        with open(temp_xml_path, "wb") as f:
            f.write(compiled_xml.encode("utf-8"))

        start_time = time.time()
        subprocess.check_call(
            [
                os.path.join(shotcut_home, "melt.exe"),
                "-progress",
                temp_xml_path,
                'real_time="0"'
            ]
        )
        end_time = time.time()
        print(f"Time: {end_time - start_time}s")
    finally:
        if delete_output_movie:
            os.remove(output_movie)
        os.remove(temp_xml_path)


if __name__ == "__main__":
    main()