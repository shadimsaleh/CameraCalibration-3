Camera Calibration program based on OpenCV sample program

You can easily use this program based on following steps.

1. Set the environmental value as follows;
   For OpenCV directory
   Name     : OpenCV_DIR
   Parameter: c:\opencv\build     <- it depends on your environment.

   For OpenCV running
   Name     : PATH
   Parameter: c:\opencv\build\bin\Debug;c:\opencv\build\bin\Release;
   
   After setting the parameter, please reboot your computer.
   
2. Printout the chess pattern for calibration
3. Take pictures of chess pattern at several directions (more than 15 images.)
4. Put taken image files to somewhere. At this time, file name should be numbered as following role.
   ex image000.png, image001.png
5. Modified "calib_cfg.yml" to fit your calibration environment.
   image_num: image number for calibration
   pattern_row: row number of chess pattern
   pattern_col: colmun number of chess pattern
   chessboard_size: the size of chessboard. the unit is [mm]
   image_stored_directory: place of calibration image stored directory
   image_file_label: image file label (if image file name is "image000.png", "label" denote "image")
   image_file_type: image file format
   output_file_name: output file name includng calibration result. the file type is ".yml".
   
You can directory use the output file to Web Camera RT components.
   
