Camera Calibration program based on OpenCV sample program

You can easily use this program as follows:

1. Printout the chess pattern for calibration
2. Take pictures of chess pattern at several directions (more than 15 images.)
3. Put taken image files to somewhere. At this time, file name should be numbered as following role.
   ex image000.png, image001.png
4. Modified "calib_cfg.yml" to fit your calibration environment.
   image_num: image number for calibration
   pattern_row: row number of chess pattern
   pattern_col: colmun number of chess pattern
   chessboard_size: the size of chessboard. the unit is [mm]
   image_stored_directory: place of calibration image stored directory
   image_file_label: image file label (if image file name is "image000.png", "label" denote "image")
   image_file_type: image file format
   output_file_name: output file name includng calibration result. the file type is ".yml".
   
You can directory use the output file to Web Camera RT components.
   
