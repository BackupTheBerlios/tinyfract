#!/bin/sh
#\
exec wish "$0" "$@"


## Include Iwidgets
package require Itcl
package require Itk
package require Iwidgets

wm title . "Tinyfract GUI"

## Set a bind command, its used remap the X11 window.
#wm protocol . WM_TAKE_FOCUS { remap $TINYFRACT_FD }
#bind . <Activate> { remap $TINYFRACT_FD }
#bind . <Deactivate> { remap $TINYFRACT_FD }
#bind . <Configure> { remap $TINYFRACT_FD }
#bind . <Map> { remap $TINYFRACT_FD }
#bind . <Unmap> { remap $TINYFRACT_FD }
#bind . <Visibility> { remap $TINYFRACT_FD }
#bind . <Expose> { remap $TINYFRACT_FD }
#bind . <Property> { remap $TINYFRACT_FD }
#bind . <Colormap> { remap $TINYFRACT_FD }

## Create safe interpreter
set parser [ interp create -safe ]

## Set default width and height
set def_width [ expr [ winfo screenwidth . ] / 2 ]
set def_height $def_width

## Set Standard parameters
set fractal mandelbrot
set geometry "${def_width}x${def_height}"
set plugin_path "/usr/local/lib/tinyfract"
set output_method x11
set output_parameter "H10,.5S10,.5B10,.5"
set h_mod 10
set h_thres 0.5
set b_mod 10
set b_thres 0.5
set s_mod 10
set s_thres 0.5
set render_method recurse
set render_parameter "3"
set precision "100"
set scale "4"
set center_imaginary 0
set center_real 0
set iteration_steps "100"
set fractal_parameter ""
set zoom_faktor 10

## Set the flags
set TINYFRACT_FD 0
set scale_test 0
set HARD_RENDER_FLAG 1


## The list which includes all movie parameter
set movie_list [ list "" ]

## Set the geometry for .
wm geometry . [ winfo screenwidth . ]x[winfo screenheight . ]

## Withdraw .
#wm withdraw .


## Use nice colors for the interface.
#option add *Menubutton.background lightgray widgetDefault
#option add *Menubutton.foreground black widgetDefault
#option add *Label.background lightgray widgetDefault
#option add *Label.foreground black widgetDefault
#option add *Entry.background lightgray widgetDefault
#option add *Entry.foreground black widgetDefault
#option add *Button.background lightgray widgetDefault
#option add *Button.foreground black widgetDefault
#option add *Menu.background lightgray widgetDefault
#option add *Menu.foreground black widgetDefault
#option add *Scale.width 10 widgetDefault
#option add *Scale.command { display_output_parameter } widgetDefault


## Error dialog.
iwidgets::messagedialog .md \
	-title "Tinyfract error!" \
	-bitmap error \
	-modality application
.md hide Help
.md buttonconfigure OK -text "OK"
.md buttonconfigure Cancel -text "Cancel"
.md deactivate


## File dialog.
iwidgets::extfileselectiondialog .file \
	-modality application
.file deactivate




## Necessary functions for recording a fractal movie
## Flags
set rec_test 0
set movie_flag 0

## Slave interpreter for recording
set rec_interp [ interp create -safe ]

## A special function which gets only the for a movie necessary parameter from an input file
proc define_rec { name value } \
{
	global rec_test
        if { $name != "center_real" && $name != "center_imaginary" && $name != "scale" && $name != "fractal" } { return }

        puts $rec_test
        ## Check if we handle the first or the last movie frame and set the options
        if { $rec_test == 0 } \
        {
                set name [ list $name 1 ]
                set name [ join $name "" ]
                puts $name
                global $name
                set $name $value
        } else \
        {
                set name [ list $name 2 ]
                set name [ join $name "" ]
                puts $name
                global $name
                set $name $value
        }
}
$rec_interp alias define define_rec



## Dialog for recording a fractal movie
toplevel .record
iwidgets::labeledwidget .record.first -labeltext "First picture"
set win1 [ .record.first childsite ]
entry $win1.entry 
button $win1.button -text "Durchsuchen" -command { $win1.entry insert 0 [ tk_getOpenFile ] }

iwidgets::labeledwidget .record.second -labeltext "Second picture"
set win2 [ .record.second childsite ]
entry $win2.entry 
button $win2.button -text "Durchsuchen" -command { $win2.entry insert 0 [ tk_getOpenFile ] }

iwidgets::labeledwidget .record.third -labeltext "Movie Name:"
set win3 [ .record.third childsite ]
entry $win3.entry

iwidgets::labeledwidget .record.forth -labeltext "Movie time:"
set win4 [ .record.forth childsite ]
iwidgets::spintime $win4.time \
	-orient horizontal

button .record.record -text "Record" -command { record_movie $TINYFRACT_FD [ open [ $win1.entry get ] r ] [ open [ $win2.entry get ] r ] [ $win3.entry get ] }
button .record.cancel -text "Cancel" -command "wm withdraw .record"

wm withdraw .record

## Dialog for displaying the progress of the movie
toplevel .movie_progress
iwidgets::feedback .movie_progress.progress \
	-labeltext "Progress from movie"
button .movie_progress.cancel \
	-text "Cancel" \
	-command "puts exit"
wm withdraw .movie_progress


## Functions
## This function gets commands from "FD" and evals them.
proc eventdata { FD } \
{
	global parser
	if {![ eof $FD ] } \
	{
		set line [ gets $FD ]
		set errno_tinyfract [ catch { $parser eval $line } result ]
		switch $errno_tinyfract \
			0 { puts "TCL_OK while execute ($line)" } \
			1 { puts "TCL_ERROR ($result) while execute ($line)" } \
			2 { puts "TCL_RETURN ($result) while execute ($line)" } \
			3 { puts "TCL_BREAK ($result) while execute ($line)" } \
			4 { puts "TCL_CONTINUE ($result) while execute ($line)" } \
			default { puts "FATAL_ERROR ($result) while execute ($line)" }
	}
}

## If an error occures this function is called
proc error_message { message yes no} \
{
	.md configure -text $message
	.md buttonconfigure OK -text $yes
	.md buttonconfigure Cancel -text $no
	if {[.md activate]} {
		puts "selected: Yes"
		return 0
	} else {
		puts "selected: No"
		return 1
	}
}

## Function which is used to remap the window. It will be called if the main window appears or disappears
proc remap { TINYFRACT_FD } \
{
	if { $TINYFRACT_FD == 0 } { return }
	puts "Remap the window"
	puts $TINYFRACT_FD u
	flush $TINYFRACT_FD
}

## Function for inerting the options into the info fields
proc insert {} \
{
	global fractal_parameter fractal plugin_path plugin_path_win output_parameter output_method render_parameter render_method geometry precision center_real center_imaginary scale iteration_steps fractal_parameter_win plugin_path_win output_parameter_win render_parameter_win geometry_win precision_win center_real_win center_imaginary_win iteration_steps_win scale_win

	## First rendering insert
	$fractal_parameter_win.entry delete 0 end
#	$output_parameter_win.entry delete 0 end
	$render_parameter_win.entry delete 0 end
	$geometry_win.entry delete 0 end
	$plugin_path_win.entry delete 0 end
	$precision_win.entry delete 0 end
	
	$fractal_parameter_win.entry insert 0 $fractal_parameter
#	$output_parameter_win.entry insert 0 $output_parameter
	$render_parameter_win.entry insert 0 $render_parameter
	$geometry_win.entry insert 0 $geometry
	$plugin_path_win.entry insert 0 $plugin_path
	$precision_win.entry insert 0 $precision

	## Insert in the comboboxes
	.right.fractal selection set $fractal
	.right.output_method selection set $output_method
	.right.render_method selection set $render_method

	## Main entries insert
	$center_real_win.entry delete 0 end
	$center_imaginary_win.entry delete 0 end
	$iteration_steps_win.entry delete 0 end
	$scale_win.entry delete 0 end
	
	$center_real_win.entry insert 0 $center_real
	$center_imaginary_win.entry insert 0 $center_imaginary
	$iteration_steps_win.entry insert 0 $iteration_steps
	$scale_win.entry insert 0 $scale

	update
}

## Function for defining variables, this function is only called in a file in wich parameters are safed
proc define { name value } \
{
	global $name

	set $name "$value"
	puts "Defined $name $value"
}
$parser alias define define

## Function for loading parameters from a file in wich parameters are safed
proc load_options {} \
{
	global parser fractal output_method render_method

	set path [ tk_getOpenFile ]
	set path_fd [ open $path "r" ]

	puts "Load $path"

	for {set line [ gets $path_fd ]} {![eof $path_fd]} {set line [ gets $path_fd ]} { $parser eval $line }
#	update
	insert
}

## Function for saving parameters in a file. Those file can be loaded by the function load_options
proc safe_options {} \
{
	global fractal plugin_path output_method h_mod h_thres b_mod b_thres s_mod s_thres render_method render_parameter precision fractal_parameter center_real_win center_imaginary_win scale_win iteration_steps_win

	set path [ tk_getSaveFile ]
	if { $path != "" } \
	{
		set path_fd [ open $path "w+" ]
	} else { return }
	
	set center_real [ $center_real_win.entry get ]
	set center_imaginary [ $center_imaginary_win.entry get ]
	set scale [ $scale_win.entry get ]
	set iteration_steps [ $iteration_steps_win.entry get ]

	puts "define fractal {$fractal}\ndefine plugin_path {$plugin_path}\ndefine output_method {$output_method}\ndefine h_mod {$h_mod}\ndefine h_thres {$h_thres}\ndefine b_mod {$b_mod}\ndefine b_thres {$b_thres}\ndefine s_mod {$s_mod}\ndefine s_thres {$s_thres}\ndefine render_method {$render_method}\ndefine render_parameter {$render_parameter}\ndefine precision {$precision}\ndefine scale {$scale}\ndefine center_real {$center_real}\ndefine center_imaginary {$center_imaginary}\ndefine iteration_steps {$iteration_steps}\ndefine fractal_parameter {$fractal_parameter}\n"

	puts $path_fd "define fractal {$fractal}\ndefine plugin_path {$plugin_path}\ndefine output_method {$output_method}\ndefine h_mod {$h_mod}\ndefine h_thres {$h_thres}\ndefine b_mod {$b_mod}\ndefine b_thres {$b_thres}\ndefine s_mod {$s_mod}\ndefine s_thres {$s_thres}\ndefine render_method {$render_method}\ndefine render_parameter {$render_parameter}\ndefine precision {$precision}\ndefine scale {$scale}\ndefine center_real {$center_real}\ndefine center_imaginary {$center_imaginary}\ndefine iteration_steps {$iteration_steps}\ndefine fractal_parameter {$fractal_parameter}\n"
	close $path_fd
}

## Function for saving as a png
proc safe_png {} \
{
	global ready_flag fractal geometry plugin_path output_method h_mod h_thres b_mod b_thres s_mod s_thres  render_method render_parameter precision fractal_parameter parser center_real_win center_imaginary_win scale_win iteration_steps_win

	set ready_flag 0

	set path [ tk_getSaveFile ]
	puts "Der Zielpfad ist $path"
	if { $path == "" } { return }
	
	set center_real [ $center_real_win.entry get ]
	set center_imaginary [ $center_imaginary_win.entry get ]
	set scale [ $scale_win.entry get ]
	set iteration_steps [ $iteration_steps_win.entry get ]

	if { $fractal_parameter == "" } \
	{
		puts "./tinyfract -f$fractal -g$geometry -P$plugin_path -opng -OH${h_mod},${h_thres}B${b_mod},${b_thres}S${s_mod},${s_thres}-$path -r$render_method -R$render_parameter -p$precision"
		set png [ open "|./tinyfract -f$fractal -g$geometry -P$plugin_path -opng -OH${h_mod},${h_thres}B${b_mod},${b_thres}S${s_mod},${s_thres}-$path -r$render_method -R$render_parameter -p$precision" "r+" ]
		fileevent $png readable "eventdata $png"
	} else \
	{
		puts "./tinyfract -f$fractal -g$geometry -P$plugin_path -opng -OH${h_mod},${h_thres}B${b_mod},${b_thres}S${s_mod},${s_thres}-$path -r$render_method -R$render_parameter -p$precision -F$fractal_parameter"
		set png [ open "|./tinyfract -f$fractal -g$geometry -P$plugin_path -opng -OH${h_mod},${h_thres}B${b_mod},${b_thres}S${s_mod},${s_thres}-$path -r$render_method -R$render_parameter -p$precision -F$fractal_parameter" "r+" ]
		fileevent $png readable "eventdata $png"
	}

	puts $png "p$center_real,$center_imaginary\ns$scale\ni$iteration_steps\nr\n"
	flush $png
	tkwait variable ready_flag
	catch { close $png }
}

## Function wich records a fractal movie
#####################################################################################################################################################################Diese Funktion muss bearbeitet werden!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!##################################################################################################################################################################################################
proc record_movie { TINYFRACT_FD first_fd second_fd name } \
{
	global win4 rec_interp rec_test center_real1 center_imaginary1 scale1 center_real2 center_imaginary2 scale2 fractal1 center_real center_imaginary scale fractal plugin_path output_parameter render_method render_parameter precision  movie_list movie_flag ready_flag

        ## Get time string and split it
        set time [ $win4.time get ]
        set time [ split $time ":" ]

        ## Calculate the number of necessary frames for the movie from the time
        set steps [ expr ( [ lindex $time 0 ] * 60 * 60 * 25 ) + ( [ lindex $time 1 ] * 60 * 25 ) + ( [ lindex $time 2 ] * 25 ) ]

        ## Get params for the movie (scale,center,...)
        set rec_test 0
        for { set line [ gets $first_fd ] } { [ eof $first_fd ] != 1 } { set line [ gets $first_fd ] } { $rec_interp eval $line }
        set rec_test 1
        for { set line [ gets $second_fd ] } { [ eof $second_fd ] != 1 } { set line [ gets $second_fd ] } { $rec_interp eval $line }

        ## Get a list from tinyfract which includes ll informations for all frames
        set movie_list ""
        set movie_flag 0
        set mov1 [ open "|./tinyfract -fmandelbrot -g100x100 -oaa -rrecurse -R3 -P./plugins -p100" "r+" ]
        fileevent $mov1 readable "eventdata $mov1"
        puts "c$center_real1,$center_imaginary1,$center_real2,$center_imaginary2,$scale1,$scale2,$steps"
        puts $mov1 "c$center_real1,$center_imaginary1,$center_real2,$center_imaginary2,$scale1,$scale2,$steps"
        flush $mov1
        tkwait variable movie_flag
        puts $mov1 "q\n"
        flush $mov1
        catch { close $mov1 }

        ## Go Recording!
        puts "Go Recording!"
        set fractal $fractal1
        puts "./tinyfract -f$fractal -P$plugin_path -g352x288 -ompeg -O${output_parameter}-$name -r$render_method -R$render_parameter -p$precision"
        set mov [ open "|./tinyfract -f$fractal -P$plugin_path -g352x288 -ompeg -O${output_parameter}-$name -r$render_method -R$render_parameter -p$precision" "r+" ]
        fileevent $mov readable "eventdata $mov"

        wm deiconify .movie_progress
        .movie_progress.progress configure -steps $steps
        .movie_progress.progress reset

	for { set i 0 } { $i < [ llength $movie_list ] } { incr i } \
        {
                puts "Render Frame $i/[ llength $movie_list ]"
                set ready_flag 0
                set center_real [ lindex [ lindex $movie_list $i ] 0 ]
                set center_imaginary [ lindex [ lindex $movie_list $i ] 1 ]
                set scale [ lindex [ lindex $movie_list $i ] 2 ]
                puts "p${center_real},${center_imaginary}\ns${scale}\nr\n"
                puts $mov "p${center_real},${center_imaginary}\ns${scale}\nr\n"
                flush $mov
                tkwait variable ready_flag
                .movie_progress.progress step
                insert
        }
	puts $mov "q\n"
	flush $mov
	catch { close $mov } result
	set err_file [ open "./error_file" "w+" ]
	puts $err_file $result
	catch { close $err_file }

        wm withdraw .movie_progress
}

## This function adds a parameter block to the movie list
proc add_movie_list { center_real center_imaginary scale steps } \
{
	global movie_list movie_flag

	lappend movie_list "$center_real $center_imaginary $scale"
	if { [ llength $movie_list ] == $steps } { set movie_flag 1 }
}
$parser alias add_movie_list add_movie_list

## Function for creating a progress bar
proc progress_cmd { actual total } \
{
	global ready_flag

	if { $actual == 1 } \
	{
		.right.progress reset
	}
	if { $actual == $total } \
	{
		set ready_flag 1
	}

	.right.progress configure -steps $total
	.right.progress step
}
$parser alias progress progress_cmd

## Function for reading new center and iteration steps
proc new_args_cmd { new_center_real new_center_imaginary new_iteration_steps } \
{
	global center_real center_imaginary iterations_steps

	set center_real $new_center_real
	set center_imaginary $new_center_imaginary
	set ieration_steps $new_iteration_steps

	insert
}
$parser alias new_args new_args_cmd

## Function for reading new scale
proc scale_cmd { new_scale } \
{
	global scale scale_test

	set scale $new_scale
	puts "New scale is: $scale"
	insert
	set scale_test "1"
}
$parser alias scale scale_cmd

## Function for displaying the new output_parameters in the fractal
proc display_output_parameter { x } \
{
	global h_mod h_thres b_mod b_thres s_mod s_thres TINYFRACT_FD

	if { $TINYFRACT_FD == 0 } { return }

	set color_string "OH${h_mod},${h_thres}B${b_mod},${b_thres}S${s_mod},${s_thres}"

	puts $color_string
	puts $TINYFRACT_FD
	puts $TINYFRACT_FD "$color_string"
	flush $TINYFRACT_FD
}


## This function is for rendering a new fractal if no new call of tinyfract is needed
proc soft_render { TINYFRACT_FD mode } \
{
	global scale_test center_real_win center_imaginary_win scale_win iteration_steps_win

	set zoom_faktor [ .right.faktor get ]

	puts "Ich bin 5"

	if { $TINYFRACT_FD == 0 } \
	{
		puts "Error: No Pipe"
		exit
	}

	if { $mode == 1 } \
	{
		puts $TINYFRACT_FD "d [ $scale_win.entry get ],$zoom_faktor"
		flush $TINYFRACT_FD
		puts "d [ $scale_win.entry get ],$zoom_faktor"
		tkwait variable scale_test
	}
	if { $mode == 0 } \
	{
		puts $TINYFRACT_FD "m [ $scale_win.entry get ],$zoom_faktor"
		flush $TINYFRACT_FD
		puts "m [ $scale_win.entry get ],$zoom_faktor"
		tkwait variable scale_test
	}
	set scale_test 0

	set center_real [ $center_real_win.entry get ]
	set center_imaginary [ $center_imaginary_win.entry get ]
	set scale [ $scale_win.entry get ]
	set iteration_steps [ $iteration_steps_win.entry get ]

	puts $TINYFRACT_FD "p$center_real,$center_imaginary\ns$scale\ni$iteration_steps\nr\n"
	puts "p$center_real,$center_imaginary\ns$scale\ni$iteration_steps\nr\n"

	flush $TINYFRACT_FD
}

## This function is for rendering a new fractal if a new call of tinyfract is needed
proc hard_render { mode  } \
{
	global TINYFRACT_FD test geometry fractal fractal_parameter plugin_path output_method h_mod h_thres b_mod b_thres s_mod s_thres render_method render_parameter precision geometry fractal_parameter_win output_parameter_win render_parameter_win precision_win geometry_win


	if { $precision == "" } \
	{
		error_message "You have to specify a precision!" OK Cancel
		return 1
	}

	if { $fractal == "" } \
	{
		error_message "You have to specify a fractal type!" Ok Cancel
		return 1
	}

	if { $plugin_path == "" } \
	{
		if { [ error_message "You did not specify a plugin path you can define a plugin path\nin the environmental variable TINYFRACT_PLUGIN_PATH or in the entry in this programm" "I have defined a plugin_path" "Cancel" ] == 1 } { return 1 }
	}

	if { $output_method == "" } \
	{
		if { [ error_message "You did not specifiy an output method you can difine a output method\nin the environmental variable TINYFRACT_OUTPUT_METHOD or in the entry in this programm." "I have defined an output method" "Cancel" ] == 1 } { return 1 }
	}

	if { $render_method == "" } \
	{
		if { [ error_message "You did not specifiy a render method you can difine a render method\nin the environmental variable TINYFRACT_RENDER_METHOD or in the entry in thos programm." "I have defined a render method" "Cancel" ] == 1 } { return 1 }
	}

	if { $render_parameter == "" } \
	{
		if { [ error_message "WARNING!!!: You did not specify render parameters but some render functions need parameters!!!" "This render function do not need parameters" "Cancel" ] == 1 } { return 1 }
	}
	
#	if { $output_parameter == "" } \
#	{
#		if { [ error_message "WARNING!!!: You did not specify output parameters but some output functions need parameters!!!" "This output function do not need parameters" "Cancel" ] == 1 } { return 1 }
#	}

	## If output is aa we have to set the geometry to another value
	if { $output_method == "aa" } \
	{
		set aa_x_geom [ expr [ winfo screenwidth . ] / 9 ]
		set aa_y_geom [ expr [ winfo screenwidth . ] / 11 ]
		set geometry "${aa_x_geom}x${aa_y_geom}" 
	}

	## Call tinyfract with standard parameters(fractal parameters are only used if given).
	if { $fractal_parameter == "" } \
	{
		set TINYFRACT_FD [ open "|./tinyfract -f$fractal -g$geometry -P$plugin_path -o$output_method -OB${b_mod},${b_thres}H${h_mod},${h_thres}S${s_mod},${s_thres}@[ winfo id .left.fractal ] -r$render_method -R$render_parameter -p$precision" "r+" ]
		puts "./tinyfract -f$fractal -g$geometry -P$plugin_path -o$output_method -OB${b_mod},${b_thres}H${h_mod},${h_thres}S${s_mod},${s_thres}@[ winfo id .left.fractal ] -r$render_method -R$render_parameter -p$precision"
	} else \
	{
		set TINYFRACT_FD [ open "|./tinyfract -f$fractal -F$fractal_parameter -g$geometry -P$plugin_path -o$output_method -OB${b_mod},${b_thres}H${h_mod},${h_thres}S${s_mod},${s_thres}@[ winfo id .left.fractal ] -r$render_method -R$render_parameter -p$precision" "r+" ]
		puts "./tinyfract -f$fractal -F$fractal_parameter -g$geometry -P$plugin_path -o$output_method -OB${b_mod},${b_thres}H${h_mod},${h_thres}S${s_mod},${s_thres}p@[  winfo id .left.fractal ] -r$render_method -R$render_parameter -p$precision"
	}

	fileevent $TINYFRACT_FD readable { eventdata $TINYFRACT_FD }

	soft_render $TINYFRACT_FD $mode

	wm deiconify .
	return 0
}

## This function decides if tinyfract has to be called new or not
proc render { TINYFRACT_FD mode } \
{
	global HARD_RENDER_FLAG fractal_parameter render_parameter plugin_path precision old_fractal_parameter old_render_parameter old_plugin_path old_precision fractal_parameter_win output_parameter_win render_parameter_win precision_win

	## Get the parameters
	set fractal_parameter [ $fractal_parameter_win.entry get ]
#	set output_parameter [ $output_parameter_win.entry get ]
	set render_parameter [ $render_parameter_win.entry get ]
	set precision [ $precision_win.entry get ]
	
	## Check if we have to open a new pipe to tinyfract
	if { $HARD_RENDER_FLAG == 0 && $fractal_parameter == $old_fractal_parameter && $render_parameter == $old_render_parameter && $plugin_path == $old_plugin_path && $precision == $old_precision } \
	{
		## NO, its no new pipe to tinyfract needed. So we call the "soft" render function
		puts "Soft render"

		soft_render $TINYFRACT_FD $mode
	} else \
	{
		## YES, its a new pipe to tinyfract needed. So we call the "hard" render function
		puts "Hard render"
		## Set first the new old parameters
		set old_fractal_parameter $fractal_parameter
#		set old_output_parameter $output_parameter
		set old_render_parameter $render_parameter
		set old_plugin_path $plugin_path
		set old_precision $precision

		## Second we have to close the old pipe
#		catch { puts $TINYFRACT_FD "q\n" }
#		catch { flush $TINYFRACT_FD }
#		catch { close $TINYFRACT_FD }

		## Now we have to set the flag back to 0.
		set HARD_RENDER_FLAG 0

		## Now call the "hard" render function
		hard_render $mode
	}
}


## Layout
## Create helper frames
frame .left
frame .right
frame .topic

frame .right.buttons

## Entries and info fields for the "hard" parameters
## File selection dialog
iwidgets::extfileselectiondialog .efsd -modality application

## Params insert fields
iwidgets::combobox .right.fractal \
	-labeltext "Fraktal type:" \
	-labelpos w \
	-selectioncommand { set fractal [ .right.fractal getcurselection ] ; set HARD_RENDER_FLAG 1 }
.right.fractal insert list end mandelbrot julia lambda
.right.fractal selection set $fractal
iwidgets::labeledwidget .right.fractal_parameter \
	-labeltext "Fractal parameter:" \
	-labelpos w
set fractal_parameter_win [ .right.fractal_parameter childsite ]
entry $fractal_parameter_win.entry
iwidgets::combobox .right.output_method \
	-labeltext "Output method:" \
	-labelpos w \
	-selectioncommand { set output_method [ .right.output_method getcurselection ] ; set HARD_RENDER_FLAG 1 }
.right.output_method insert list end x11 aa
.right.output_method selection set $output_method
#iwidgets::labeledwidget .right.output_parameter \
#	-labeltext "Output parameter:" \
#	-labelpos w
#set output_parameter_win [ .right.output_parameter childsite ]
#entry $output_parameter_win.entry
iwidgets::combobox .right.render_method \
	-labeltext "Render method:" \
	-labelpos w \
	-selectioncommand { set render_method [ .right.render_method getcurselection ] ; set HARD_RENDER_FLAG 1 }
.right.render_method insert list end recurse dumb pix
.right.render_method selection set $render_method
iwidgets::labeledwidget .right.render_parameter \
	-labeltext "Render parameter:" \
	-labelpos w
set render_parameter_win [ .right.render_parameter childsite ]
entry $render_parameter_win.entry
iwidgets::labeledwidget .right.geometry \
	-labeltext "Geometry (your geometry: [ winfo screenwidth . ]x[winfo screenheight . ]):" \
	-labelpos w
set geometry_win [ .right.geometry childsite ]
entry $geometry_win.entry
iwidgets::labeledwidget .right.plugin_path \
	-labeltext "Plugin path:" \
	-labelpos w
set plugin_path_win [ .right.plugin_path childsite ]
entry $plugin_path_win.entry
button $plugin_path_win.button \
	-text "Files" \
	-command {
		set plugin_path [ tk_chooseDirectory ] ; insert
	}
iwidgets::labeledwidget .right.precision \
	-labeltext "Precision:" \
	-labelpos w
set precision_win [ .right.precision childsite ]
entry $precision_win.entry



## Buttons wich start, load or cancel
#button .right.start_tinyfract \
#	-text "OK" \
#	-command { first_rendering }



## Headline
label .topic.headline \
	-text "Graphical user interface for tinyfract" \
	-font "-adobe-courier-bold-r-*-*-34-*-100-100-n-200-iso8859-1" \
	-fg black

## Entries and info fields for the "soft" parameters
iwidgets::labeledwidget .right.center_real \
	-labeltext "Center (Reeler Teil):" \
	-labelpos w
set center_real_win [ .right.center_real childsite ]
entry $center_real_win.entry

iwidgets::labeledwidget .right.center_imaginary \
	-labeltext "Center (Imaginärer Reil):" \
	-labelpos w
set center_imaginary_win [ .right.center_imaginary childsite ]
entry $center_imaginary_win.entry

iwidgets::labeledwidget .right.iteration_steps\
	-labeltext "Iteration Steps:" \
	-labelpos w
set iteration_steps_win [ .right.iteration_steps childsite ]
entry $iteration_steps_win.entry

iwidgets::labeledwidget .right.scale \
	-labeltext "Scale:" \
	-labelpos w
set scale_win [ .right.scale childsite ]
entry $scale_win.entry


## Build Widgets in the left frame
## Frame for displaing the fractal
frame .left.fractal \
	-width $def_width \
	-height $def_height \
	-container 1

## Output Prameters scales
iwidgets::labeledwidget .left.h \
	-labeltext "H:" \
	-labelpos w

set h_win [ .left.h childsite ]

scale $h_win.scale_mod \
	-from 0 \
	-to 100 \
	-resolution 1 \
	-digits 1 \
	-variable h_mod \
	-label "Mod:" \
	-orient horizontal

scale $h_win.scale_thres \
	-from 0 \
	-to 1 \
	-resolution 0.01 \
	-digits 3 \
	-variable h_thres \
	-label "Thres:" \
	-orient horizontal


iwidgets::labeledwidget .left.b \
	-labeltext "B:" \
	-labelpos w

set b_win [ .left.b childsite ]

scale $b_win.scale_mod \
	-from 0 \
	-to 100 \
	-resolution 1 \
	-digits 1 \
	-variable b_mod \
	-label "Mod:" \
	-orient horizontal


scale $b_win.scale_thres \
	-from 0 \
	-to 1 \
	-resolution 0.01 \
	-digits 3 \
	-variable b_thres \
	-label "Thres" \
	-orient horizontal


iwidgets::labeledwidget .left.s \
	-labeltext "S:" \
	-labelpos w

set s_win [ .left.s childsite ]

scale $s_win.scale_mod \
	-from 0 \
	-to 100 \
	-resolution 1 \
	-digits 1 \
	-variable s_mod \
	-label "Mod:" \
	-orient horizontal


scale $s_win.scale_thres \
	-from 0 \
	-to 1 \
	-resolution 0.01 \
	-digits 3 \
	-variable s_thres \
	-label "Thres:" \
	-orient horizontal



## Build the progress bar
iwidgets::feedback .right.progress \
	-labeltext "Progress"

## Build Buttons for auto zooming
iwidgets::spinint .right.faktor \
	-labeltext "Zoom Faktor" \
	-range { 0 1000 }
.right.faktor delete 0 end
.right.faktor insert 0 $zoom_faktor
button .right.buttons.zoom_in \
	-text "+" \
	-command { render $TINYFRACT_FD 1 }
button .right.buttons.move \
	-text "<-->" \
	-command { render $TINYFRACT_FD 3 }
button .right.buttons.zoom_out \
	-text "-" \
	-command { render $TINYFRACT_FD 0 }

## This is just a seperator, to divide the "hard" from the "soft" parameters
frame .right.seperator \
	-background black \
	-relief sunken \
	-height 1

## Insert the default values
insert

## Create the menue on the top
## Main menu
menu .main

## Submenu "File"
menu .main.file -title "Datei"

.main.file add command -command "load_options" -label "Öffnen"
.main.file add separator
.main.file add command -command "safe_options" -label "Speichern"
.main.file add command -command "safe_png" -label "Als PNG speichern"
.main.file add command -command "wm deiconify .record" -label "Einen Fraktal-Film aufnehmen"
.main.file add separator
.main.file add command -command "puts print" -label "Drucken"
.main.file add separator
.main.file add command -command { puts $TINYFRACT_FD q ; flush $TINYFRACT_FD ; catch { close $TINYFRACT_FD } ; exit }  -label "Beenden"

## Add the submenues to the main menu
.main add cascade -menu .main.file -label "Datei"

## Add the main menu to the toplevel
. configure -menu .main


############################################ Ich glaub das kann weg!!! ##################################################################################
## Error dialog
#toplevel .error
#label .error.bitmap -bitmap error
#label .error.message -text "test"
#button .error.exit -text "OK" -command "wm withdraw .error"
#button .error.return -text "Cancel" -command {wm withdraw .error}
#bind .error.return <ButtonPress> {return}
#bind .error.exit <ButtonPress> "set test 5"
#bind .error.return <ButtonPress> "set test 5"

#wm withdraw .error

## Pack all widgets
#grid .error.message -row 0 -column 1 -sticky nsew
#grid .error.bitmap -row 0 -column 0 -sticky nsew
#grid .error.exit -row 1 -column 1 -sticky nsew
#grid .error.return -row 1 -column 0 -sticky nsew

############################################### Bis hier ################################################################################################



## Main window
## Pack Headline
pack .topic.headline -expand 1 -fill both


## Pack widgets in the right frame
pack .right.fractal -fill both -expand 1
pack .right.fractal_parameter -fill both -expand 1
pack $fractal_parameter_win.entry -side left -fill both -expand 1
pack .right.output_method -fill both -expand 1
#pack .right.output_parameter -fill both -expand 1
#pack $output_parameter_win.entry -side left -fill both -expand 1
pack .right.render_method  -fill both -expand 1
pack .right.render_parameter -fill both -expand 1
pack $render_parameter_win.entry -side left -fill both -expand 1
#pack .right.geometry -fill both -expand 1
#pack $geometry_win.entry -side left -fill both -expand 1
pack .right.plugin_path -fill both -expand 1
pack $plugin_path_win.entry -side left -fill both -expand 1
pack $plugin_path_win.button -side left -fill both -expand 1
pack .right.precision -fill both -expand 1
pack $precision_win.entry -side left -fill both -expand 1

pack .right.seperator -fill x

pack .right.center_real -side top -expand 1 -fill both
pack $center_real_win.entry -expand 1 -fill both
pack .right.center_imaginary -side top -expand 1 -fill both
pack $center_imaginary_win.entry -expand 1 -fill both
pack .right.scale -side top -expand 1 -fill both
pack $scale_win.entry -expand 1 -fill both
pack .right.iteration_steps -side top -expand 1 -fill both
pack $iteration_steps_win.entry -expand 1 -fill both

pack .right.faktor -expand 1 -fill both -side top

pack .right.buttons.zoom_in -side left -expand 1 -fill both
pack .right.buttons.move -side left -expand 1 -fill both
pack .right.buttons.zoom_out -side left -expand 1 -fill both

pack .right.buttons -expand 1 -fill both

#pack .right.start_tinyfract -side top -fill both -expand 1

pack .right.progress -expand 1 -fill both


## Pack Widgets in the left frame
pack .left.h -expand 1 -fill x
pack $h_win.scale_mod -expand 1 -fill x -side left
pack $h_win.scale_thres -expand 1 -fill x -side left

pack .left.s -expand 1 -fill x
pack $s_win.scale_mod -expand 1 -fill x -side left
pack $s_win.scale_thres -expand 1 -fill x -side left

pack .left.b -fill x
pack $b_win.scale_mod -expand 1 -fill x -side left
pack $b_win.scale_thres -expand 1 -fill x -side left


pack .left.fractal


## Pack the Record-Dialog widgets
pack .record.first -expand 1 -fill both
pack .record.second -expand 1 -fill both
pack .record.third -expand 1 -fill both
pack .record.forth -expand 1 -fill both
pack $win1.entry -expand 1 -fill both
pack $win1.button -expand 1 -fill both
pack $win2.entry -expand 1 -fill both
pack $win2.button -expand 1 -fill both
pack $win3.entry -expand 1 -fill both
pack $win4.time -expand 1 -fill both
pack .record.record -expand 1 -fill both
pack .record.cancel -expand 1 -fill both

pack .movie_progress.progress -expand 1 -fill both
pack .movie_progress.cancel -expand 1 -fill both




## Pack the helper frames
pack .topic -expand 1 -fill both
pack .right -side right -expand 1 -fill both
pack .left -side left













