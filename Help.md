# Manual #
  * Install and First Startup
  * Creating a Rider
  * Retrieving Log Files
  * General Usage and Viewing a Ride
  * Tools

## 1 Install and First Startup ##
Download the installer and use this to install on your system. Once RiderViewer has been installed, and you launch it for the first time, you will be prompted to create a new rider profile. After creating a user, you can retrieve log files from your Garmin device.

## 2 Creating a Rider ##
Enter your details and select a log directory where you want to have all your log files stored. RiderViewer will copy the files from your device to this location (.fit) or you need to manually place the files here (.tcx).

## 3 Retrieving Log Files ##
If you are using a Garmin device, you can use the instructions in 3.1. If you are using any other device, you need to use 3rd party tools to convert the logs to .tcx format and follow instructions in 3.2.

### 3.1 Garmin Fit Files (.fit) ###
First ensure your Garmin device is plugged-in and recognised by your system (ie. wait 10-20 seconds after plugging-in). Then, you can retrieve the .fit files from your device using the Actions->Retrieve Logs option from RideViewer. This will copy any logs on your Garmin device which are not already in your defined log directory. Note, this will copy _all_ the files if your log directory is empty.

### 3.2 XML Log Files (.tcx) ###
If you only have access to .tcx files, there is a little bit of manual work here. You need to copy any new logs to the log directory you specified in the user profile. Copy these logs before you start RideViewer so that RideViewer will synchronise to those files on startup.

### 3.3 Note ###
RideViewer will _not_ modify any of the logs! It only reads them. But it will create an additional file in the log directory to summaries their details.

You can modify logs using the Tools->Log File Editor, but this will always make a copy of the log before saving your file. So you will never lose data.

## 4 General Usage and Viewing a Ride ##
Once you have retrieved all your rides, you can select which ride to view by using the top left selection dialogue. All logs which are available are shown in this dialogue. After selecting a ride, it will be loaded and all available details with be presented in the 3 _view_ components: graphical plots, Google maps overview, and ride statistics. The functionality should be intuitive (hopefully!). Move the cursor over the 2d graphical plots and zoom in on sections of interest (left click + drag = zoom in, right click = zoom out). When you zoom, the statistics window will update with statistics for the portion of the ride you have selected. You can colour the path in the Google maps overview according to certain parameters of the ride, eg speed, heart rate etc. This is a nice way to visualise where you were making your efforts.

## 5 Tools ##

### 5.1 Totals ###
This presents metrics for all the ride logs in your directory - distance and time. You can select how you want to view the statistics: weekly, monthly, or yearly.

![http://img442.imageshack.us/img442/659/totalsmetric.png](http://img442.imageshack.us/img442/659/totalsmetric.png)

### 5.2 Ride Collage ###
This plots all your rides on a Google map simultaneously, colouring the points according to how frequently you ride that part of the route. It is a nice way to see where you are riding most frequently, and what terrain you may have missed in your training area.

Warning, due to the large amount of data, producing this plot can be slow, so please be patient!

http://desmond.imageshack.us/Himg12/scaled.php?server=12&filename=screenshot2gg.png&res=medium

### 5.3 Ride Interval Finder ###
This is a great tool for comparing your efforts from one ride to all other times you have ridden the same section. You simply select the portion of a ride which you are interested in (from the 2d plot viewer), and run the Rider Interval Finder tool.

![http://img541.imageshack.us/img541/2097/intervalfinder.png](http://img541.imageshack.us/img541/2097/intervalfinder.png)

### 5.4 Log File Editor ###
This tool enables you to make simple edits to log files and even split them in two if needed.

  * You can use the search function to find values which maybe of interest.

  * You can edit the values in any of the fields and save the changes. This will backup your original log file to _original name_.orig and save the changes you made in a new file called the same as the original file, ie. _original name_.fit.

  * You can split a file in two if for some reason it is needed (sometimes one doesn't reset the Garmin device, etc). Select the index where you want to split the log. The index will be the first data point in the second portion of the split. Your original file (non-split) will be renamed _original name_.orig and two split files will be created: _original name_.fit and _original name_ pt2.fit.

![http://img132.imageshack.us/img132/8734/logeditor.png](http://img132.imageshack.us/img132/8734/logeditor.png)