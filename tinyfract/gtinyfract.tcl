#!/usr/bin/wish

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

## Set default width height
set def_width [ expr [ winfo screenwidth . ] / 2 ]
set def_height $def_width

## Standard parameters
set fractal mandelbrot
set geometry "${def_width}x${def_height}"
set plugin_path "$env(PWD)/plugins"
set output_method x11
set output_parameter "H10,.5S10,.5B10,.5"
set render_method recurse
set render_parameter "3"
set precision "100"
set scale "4"
set scale_test 0
set center_real 0
set center_imaginary 0
set iteration_steps "100"
set fractal_parameter ""
set zoom_faktor 10

set TINYFRACT_FD 0

## Movie list
set movie_list [ list "" ]

## Set the geometry for .
wm geometry . [ winfo screenwidth . ]x[winfo screenheight . ]

## Withdraw .
wm withdraw .

## Make nice colors for the interface
option add *Menubutton.background lightgrey 100
option add *Menubutton.foreground black 100

option add *Label.background lightgrey 100
option add *Label.foreground black 100

option add *Entry.background lightgrey 100
option add *Entry.foreground black 100

option add *Button.background lightgrey 100
option add *Button.foreground black 100

option add *Menu.background lightgrey 100
option add *Menu.foreground black 100

## Dialog for an error
iwidgets::messagedialog .md \
	-title "Error" \
	-bitmap error \
	-modality application
.md hide Help
.md buttonconfigure OK -text "OK"
.md buttonconfigure Cancel -text "Cancel"
.md deactivate

## Dialog for searching a file
iwidgets::extfileselectiondialog .file \
	-modality application
.file deactivate

## Record a fractal movie
## Volatile datas
set rec_test 0
set movie_flag 0

## Slave interpreter for recording
set rec_interp [ interp create -safe ]

proc define_rec { name value } \
{
	global rec_test
	if { $name != "center_real" && $name != "center_imaginary" && $name != "scale" } { return }

	puts $rec_test
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
## Function which is used to remap the window
proc remap { TINYFRACT_FD } \
{
	if { $TINYFRACT_FD == 0 } { return }
	puts "Remap the window"
	puts $TINYFRACT_FD u
	flush $TINYFRACT_FD
}

## Function for inerting options
proc insert {} \
{
	global fractal_parameter fractal plugin_path plugin_path_win output_parameter output_method render_parameter render_method geometry precision center_real center_imaginary scale iteration_steps fractal_parameter_win plugin_path_win output_parameter_win render_parameter_win geometry_win precision_win

	## First rendering insert
	$fractal_parameter_win.entry delete 0 end
	$output_parameter_win.entry delete 0 end
	$render_parameter_win.entry delete 0 end
	$geometry_win.entry delete 0 end
	$plugin_path_win.entry delete 0 end
	$precision_win.entry delete 0 end
	
	$fractal_parameter_win.entry insert 0 $fractal_parameter
	$output_parameter_win.entry insert 0 $output_parameter
	$render_parameter_win.entry insert 0 $render_parameter
	$geometry_win.entry insert 0 $geometry
	$plugin_path_win.entry insert 0 $plugin_path
	$precision_win.entry insert 0 $precision

	## Main entries insert
	.left.center_real.center_real delete 0 end
	.left.center_imaginary.center_imaginary delete 0 end
	.left.iterations.iteration_steps delete 0 end
	.left.scale.scale delete 0 end
	
	.left.center_real.center_real insert 0 $center_real
	.left.center_imaginary.center_imaginary insert 0 $center_imaginary
	.left.iterations.iteration_steps insert 0 $iteration_steps
	.left.scale.scale insert 0 $scale

	## Update the misc info fields
	.misc.fractal configure \
		-text "Fraktal: $fractal"
	.misc.fractal_parameter configure \
		-text "Fraktal Parameter: $fractal_parameter"
	.misc.render_method configure \
		-text "Render Art: $render_method"
	.misc.render_args configure \
		-text "Render Parameter: $render_parameter"
	.misc.prec configure \
		-text "Precision: $precision"
	.misc.plugin_path configure \
		-text "Plugin path: $plugin_path"

	update
}

## Function for defining variables
proc define { name value } \
{
	global $name

	set $name "$value"
	puts "Defined $name $value"
}
$parser alias define define

proc add_movie_list { center_real center_imaginary scale steps } \
{
	global movie_list movie_flag

	lappend movie_list "$center_real $center_imaginary $scale"
	if { [ llength $movie_list ] == $steps } { set movie_flag 1 }
}
$parser alias add_movie_list add_movie_list

## Function for loading
proc load_options {} \
{
	global parser fractal output_method render_method

	set path [ tk_getOpenFile ]
	set path_fd [ open $path "r" ]

	for {set line [ gets $path_fd ]} {![eof $path_fd]} {set line [ gets $path_fd ]} { $parser eval $line }
	update
	insert
}

## Function for saving
proc safe_options {} \
{
	global fractal plugin_path output_method output_parameter render_method render_parameter precision fractal_parameter

	set path [ tk_getSaveFile ]
	if { $path != "" } \
	{
		set path_fd [ open $path "w+" ]
	} else { return }
	
	set center_real [ .left.center_real.center_real get ]
	set center_imaginary [ .left.center_imaginary.center_imaginary get ]
	set scale [ .left.scale.scale get ]
	set iteration_steps [ .left.iterations.iteration_steps get ]

	puts "define fractal {$fractal}\ndefine plugin_path {$plugin_path}\ndefine output_method {$output_method}\ndefine output_parameter {$output_parameter}\ndefine render_method {$render_method}\ndefine render_parameter {$render_parameter}\ndefine precision {$precision}\ndefine scale {$scale}\ndefine center_real {$center_real}\ndefine center_imaginary {$center_imaginary}\ndefine iteration_steps {$iteration_steps}\ndefine fractal_parameter {$fractal_parameter}\n"
	puts $path_fd "define fractal {$fractal}\ndefine plugin_path {$plugin_path}\ndefine output_method {$output_method}\ndefine output_parameter {$output_parameter}\ndefine render_method {$render_method}\ndefine render_parameter {$render_parameter}\ndefine precision {$precision}\ndefine scale {$scale}\ndefine center_real {$center_real}\ndefine center_imaginary {$center_imaginary}\ndefine iteration_steps {$iteration_steps}\ndefine fractal_parameter {$fractal_parameter}\n"
	close $path_fd
}

## Function which evals options
proc eventdata { TINYFRACT_FD } \
{
	global parser
	if {![ eof $TINYFRACT_FD ] } \
	{
		set line [ gets $TINYFRACT_FD ]
		puts "Command is: ($line)"
		if { [ catch [ $parser eval $line ] result ] != 0 } { puts "While executing ($line) this error occured:($result)" }	
	}
}

## Function for saving as a png
proc safe_png {} \
{
	global ready_flag fractal geometry plugin_path output_method output_parameter render_method render_parameter precision fractal_parameter parser

	set ready_flag 0

	set path [ tk_getSaveFile ]
	puts "Der Zielpfad ist $path"
	if { $path == "" } { return }
	
	set center_real [ .left.center_real.center_real get ]
	set center_imaginary [ .left.center_imaginary.center_imaginary get ]
	set scale [ .left.scale.scale get ]
	set iteration_steps [ .left.iterations.iteration_steps get ]

	if { $fractal_parameter == "" } \
	{
		puts "./tinyfract -f$fractal -g$geometry -P$plugin_path -opng -O$output_parameter-$path -r$render_method -R$render_parameter -p$precision"
		set png [ open "|./tinyfract -f$fractal -g$geometry -P$plugin_path -opng -O$output_parameter-$path -r$render_method -R$render_parameter -p$precision" "r+" ]
		fileevent $png readable "eventdata $png"
	} else \
	{
		puts "./tinyfract -f$fractal -g$geometry -P$plugin_path -opng -O$output_parameter-$path -r$render_method -R$render_parameter -p$precision -F$fractal_parameter"
		set png [ open "|./tinyfract -f$fractal -g$geometry -P$plugin_path -opng -O$output_parameter-$path -r$render_method -R$render_parameter -p$precision -F$fractal_parameter" "r+" ]
		fileevent $png readable "eventdata $png"
	}

	puts $png "p$center_real,$center_imaginary\ns$scale\ni$iteration_steps\nr\n"
	flush $png
	tkwait variable ready_flag
	catch { close $png }
}

## Record a fractal movie
proc record_movie { TINYFRACT_FD first_fd second_fd name } \
{
	global win4 rec_interp rec_test center_real1 center_imaginary1 scale1 center_real2 center_imaginary2 scale2 movie_list movie_flag ready_flag

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

	set movie_list ""
	set movie_flag 0
	set mov1 [ open "|./tinyfract -fmandelbrot -g100x100 -oaa -rrecurse -R3 -P./plugins -p100" "r+" ]
	fileevent $mov1 readable "eventdata $mov1"
	puts "c$center_real1,$center_imaginary1,$center_real2,$center_imaginary2,$scale1,$scale2,$steps"
	puts $mov1 "c$center_real1,$center_imaginary1,$center_real2,$center_imaginary2,$scale1,$scale2,$steps"
	flush $mov1
	tkwait variable movie_flag
	puts $movie_list
	puts $mov1 "q\n"
	flush $mov1
	catch { close $mov1 }

	puts "Go Recording!"
	set mov [ open "|./tinyfract -fmandelbrot -P./plugins -g352x288 -ompeg -OH10,.5S10,.5B10,.5-mandel.mpeg -rrecurse -R3 -p100" "r+" ]
	fileevent $mov readable "eventdata $mov"

	wm deiconify .movie_progress
	.movie_progress.progress configure -steps $steps
	.movie_progress.progress reset
	for { set i 0 } { $i < [ llength $movie_list ] } { incr i } \
	{
		puts "Render Frame $i/[ llength $movie_list ]"
		set ready_flag 0
		puts "p[ lindex [ lindex $movie_list $i ] 0 ],[ lindex [ lindex $movie_list $i ] 1 ]\ns[ lindex [ lindex $movie_list $i ] 2 ]\nr\n"
		puts $mov "p[ lindex [ lindex $movie_list $i ] 0 ],[ lindex [ lindex $movie_list $i ] 1 ]\ns[ lindex [ lindex $movie_list $i ] 2 ]\nr\n"
		flush $mov
		tkwait variable ready_flag
		.movie_progress.progress step
		insert
	}
	puts $mov "q\n"
	flush $mov
	close $mov

	wm withdraw .movie_progress
}

## Function for making a progress bar
proc progress_cmd { actual total } \
{
	global ready_flag

	if { $actual == 1 } \
	{
		.buttons.progress reset
	}
	if { $actual == $total } \
	{
		set ready_flag 1
	}

	.buttons.progress configure -steps $total
	.buttons.progress step
}

$parser alias progress progress_cmd

## Function for putting new args
proc new_args_cmd { center_real center_imaginary iteration_steps } \
{
	.left.center_real.center_real delete 0 end
	.left.center_imaginary.center_imaginary delete 0 end
	.left.iterations.iteration_steps delete 0 end

	.left.center_real.center_real insert 0 "$center_real"
	.left.center_imaginary.center_imaginary insert 0 "$center_imaginary"
	.left.iterations.iteration_steps insert 0 "$iteration_steps"
}
$parser alias new_args new_args_cmd

## Function for reading new scale
proc scale { new_scale } \
{
	global scale scale_test
	
	set scale $new_scale
	.left.scale.scale delete 0 end
	.left.scale.scale insert 0 $new_scale
	puts "New scale is: $scale"
	set scale_test "1"
}
$parser alias scale scale


## An error occured
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

## Start rendering function
proc render { TINYFRACT_FD mode } \
{
	global scale_test

	set zoom_faktor [ .right.zoom.faktor get ]


	if { $TINYFRACT_FD == 0 } \
	{
		puts "Error: No Pipe"
		exit
	}

	if { $mode == 1 } \
	{
		puts $TINYFRACT_FD "d [ .left.scale.scale get ],$zoom_faktor"
		flush $TINYFRACT_FD
		puts "d [ .left.scale.scale get ],$zoom_faktor"
		tkwait variable scale_test
	}
	if { $mode == 0 } \
	{
		puts $TINYFRACT_FD "m [ .left.scale.scale get ],$zoom_faktor"
		flush $TINYFRACT_FD
		puts "m [ .left.scale.scale get ],$zoom_faktor"
		tkwait variable scale_test
	}
	set scale_test 0

	set center_real [ .left.center_real.center_real get ]
	set center_imaginary [ .left.center_imaginary.center_imaginary get ]
	set scale [ .left.scale.scale get ]
	set iteration_steps [ .left.iterations.iteration_steps get ]

	puts $TINYFRACT_FD "p$center_real,$center_imaginary\ns$scale\ni$iteration_steps\nr\n"
	puts "p$center_real,$center_imaginary\ns$scale\ni$iteration_steps\nr\n"

	flush $TINYFRACT_FD
}

## Function for first rendering
proc first_rendering {} \
{
	global test TINYFRACT_FD geometry fractal fractal_parameter plugin_path output_method output_parameter render_method render_parameter precision geometry fractal_parameter_win output_parameter_win render_parameter_win precision_win geometry_win

	set fractal_parameter [ $fractal_parameter_win.entry get ]
	set output_parameter [ $output_parameter_win.entry get ]
	set render_parameter [ $render_parameter_win.entry get ]
	set precision [ $precision_win.entry get ]
	set geometry [ $geometry_win.entry get ]
	
	update

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
	
	if { $output_parameter == "" } \
	{
		if { [ error_message "WARNING!!!: You did not specify output parameters but some output functions need parameters!!!" "This output function do not need parameters" "Cancel" ] == 1 } { return 1 }
	}

	## Call tinyfract with standard parameters(fractal parameters are only used if necessary).
	if { $fractal_parameter == "" } \
	{
		set TINYFRACT_FD [ open "|./tinyfract -f$fractal -g$geometry -P$plugin_path -o$output_method -O${output_parameter}@[ winfo id .fractal ] -r$render_method -R$render_parameter -p$precision" "r+" ]
		puts "./tinyfract -f$fractal -g$geometry -P$plugin_path -o$output_method -O$output_parameter@[ winfo id .fractal ] -r$render_method -R$render_parameter -p$precision"
	} else \
	{
		set TINYFRACT_FD [ open "|./tinyfract -f$fractal -F$fractal_parameter -g$geometry -P$plugin_path -o$output_method -O$output_parameter@[ winfo id .fractal ] -r$render_method -R$render_parameter -p$precision" "r+" ]
		puts "./tinyfract -f$fractal -F$fractal_parameter -g$geometry -P$plugin_path -o$output_method -O$output_parameter@[ winfo id .fractal ] -r$render_method -R$render_parameter -p$precision"
	}

	fileevent $TINYFRACT_FD readable { eventdata $TINYFRACT_FD }

	render $TINYFRACT_FD 3

	wm deiconify .
	return 0
}


## Make Toplevel in witch first important parameters can be given
## Build the toplevel

toplevel .start_params


## Build necessary entry and information fields
## File selection dialog
iwidgets::extfileselectiondialog .efsd -modality application
## Params insert fields
iwidgets::combobox .start_params.fractal \
	-labeltext "Fraktal type:" \
	-labelpos w \
	-selectioncommand { set fractal [ .start_params.fractal getcurselection ] }
.start_params.fractal insert list end mandelbrot julia lambda
.start_params.fractal selection set $fractal
iwidgets::labeledwidget .start_params.fractal_parameter \
	-labeltext "Fractal parameter:" \
	-labelpos w
set fractal_parameter_win [ .start_params.fractal_parameter childsite ]
entry $fractal_parameter_win.entry
iwidgets::combobox .start_params.output_method \
	-labeltext "Output method:" \
	-labelpos w \
	-selectioncommand { set output_method [ .start_params.output_method getcurselection ] }
.start_params.output_method insert list end x11 aa
.start_params.output_method selection set $output_method
iwidgets::labeledwidget .start_params.output_parameter \
	-labeltext "Output parameter:" \
	-labelpos w
set output_parameter_win [ .start_params.output_parameter childsite ]
entry $output_parameter_win.entry
iwidgets::combobox .start_params.render_method \
	-labeltext "Render method:" \
	-labelpos w \
	-selectioncommand { set render_method [ .start_params.render_method getcurselection ] }
.start_params.render_method insert list end recurse dumb pix
.start_params.render_method selection set $render_method
iwidgets::labeledwidget .start_params.render_parameter \
	-labeltext "Render parameter:" \
	-labelpos w
set render_parameter_win [ .start_params.render_parameter childsite ]
entry $render_parameter_win.entry
iwidgets::labeledwidget .start_params.geometry \
	-labeltext "Geometry (your geometry: [ winfo screenwidth . ]x[winfo screenheight . ]):" \
	-labelpos w
set geometry_win [ .start_params.geometry childsite ]
entry $geometry_win.entry
iwidgets::labeledwidget .start_params.plugin_path \
	-labeltext "Plugin path:" \
	-labelpos w
set plugin_path_win [ .start_params.plugin_path childsite ]
entry $plugin_path_win.entry
button $plugin_path_win.button \
	-text "Files" \
	-command {
		set plugin_path [ tk_chooseDirectory ] ; insert
	}
iwidgets::labeledwidget .start_params.precision \
	-labeltext "Precision:" \
	-labelpos w
set precision_win [ .start_params.precision childsite ]
entry $precision_win.entry

## Build Buttons
frame .start_params.buttons

button .start_params.buttons.start_tinyfract \
	-text "OK" \
	-command { if { [ first_rendering ] == 0 } { wm withdraw .start_params } }
button .start_params.buttons.cancel \
	-text "Cancel" \
	-command "exit"
button .start_params.buttons.load_parameter \
	-text "Load" \
	-command "load_options"


## Make labels and info fields only for center, scale and iteration steps
frame .left
frame .right

frame .topic
frame .left.center_real
frame .left.center_imaginary
frame .left.scale
frame .left.iterations
frame .right.zoom
frame .buttons


label .topic.headline \
	-text "Graphical user interface for tinyfract" \
	-font "-adobe-courier-bold-r-*-*-34-*-100-100-n-200-iso8859-1" \
	-fg black

label .left.center_real.center_real_info -text "Center(Real part):"
label .left.center_imaginary.center_imaginary_info -text "Center(Imaginary part):"
label .left.iterations.iteration_steps_info -text "Iteration steps:"
label .left.scale.scale_info -text "Scale:"

entry .left.center_real.center_real
entry .left.center_imaginary.center_imaginary
entry .left.iterations.iteration_steps
entry .left.scale.scale

## Frame for displaing the fractal
frame .fractal \
	-width $def_width \
	-height $def_height \
	-container 1

## Misceleanous informations for the user
frame .misc

label .misc.fractal \
	-text "Fraktal: $fractal"
label .misc.fractal_parameter \
	-text "Fraktal Parameter: $fractal_parameter"
label .misc.render_method \
	-text "Render Art: $render_method"
label .misc.render_args \
	-text "Render Parameter: $render_parameter"
label .misc.prec \
	-text "Precision: $precision"
label .misc.plugin_path \
	-text "Plugin path: $plugin_path"

## Insert options
insert


## Build the progress bar
iwidgets::feedback .buttons.progress \
	-labeltext "Progress"

## Build Safe, record and Cancel button
button .buttons.cancel \
	-text "Cancel" \
	-command { puts $TINYFRACT_FD q ; flush $TINYFRACT_FD ; catch { close $TINYFRACT_FD } ; exit }
button .buttons.safe \
	-text Safe \
	-command "safe_options"
button .buttons.safe_png \
	-text "Safe as png" \
	-command safe_png
button .buttons.record_movie \
	-text "Record fractal movie" \
	-command "wm deiconify .record"

## Build Buttons for auto zooming
iwidgets::spinint .right.zoom.faktor \
	-labeltext "Zoom Faktor" \
	-range { 0 1000 }
.right.zoom.faktor delete 0 end
.right.zoom.faktor insert 0 $zoom_faktor
button .right.zoom.zoom_in \
	-text "+" \
	-command { render $TINYFRACT_FD 1 }
button .right.zoom.move \
	-text "<-->" \
	-command { render $TINYFRACT_FD 3 }
button .right.zoom.zoom_out \
	-text "-" \
	-command { render $TINYFRACT_FD 0 }



## Error dialog
toplevel .error
label .error.bitmap -bitmap error
label .error.message -text "test"
button .error.exit -text "OK" -command "wm withdraw .error"
button .error.return -text "Cancel" -command {wm withdraw .error}
#bind .error.return <ButtonPress> {return}
bind .error.exit <ButtonPress> "set test 5"
bind .error.return <ButtonPress> "set test 5"

wm withdraw .error

## Pack all widgets
grid .error.message -row 0 -column 1 -sticky nsew
grid .error.bitmap -row 0 -column 0 -sticky nsew
grid .error.exit -row 1 -column 1 -sticky nsew
grid .error.return -row 1 -column 0 -sticky nsew


## Main window
pack .topic.headline -expand 1 -fill both

pack .left.center_real.center_real_info -side top -expand 1 -fill both
pack .left.center_imaginary.center_imaginary_info -side top -expand 1 -fill both
pack .left.center_real.center_real -side top -expand 1 -fill both
pack .left.center_imaginary.center_imaginary -side top -expand 1 -fill both

pack .left.iterations.iteration_steps_info -side top -expand 1 -fill both
pack .left.iterations.iteration_steps -side top -expand 1 -fill both

pack .left.scale.scale_info -side top -expand 1 -fill both
pack .left.scale.scale -side top -expand 1 -fill both

pack .right.zoom.faktor -expand 1 -fill both
pack .right.zoom.zoom_in -side left -expand 1 -fill both
pack .right.zoom.move -side left -expand 1 -fill both
pack .right.zoom.zoom_out -side left -expand 1 -fill both

pack .buttons.safe -expand 1 -fill both
pack .buttons.safe_png -expand 1 -fill both
pack .buttons.record_movie -expand 1 -fill both
pack .buttons.cancel -expand 1 -fill both
pack .buttons.progress -expand 1 -fill both

grid .misc.fractal -row 0 -column 0 -sticky nsew
grid .misc.fractal_parameter -row 0 -column 1 -sticky nsew
grid .misc.prec -row 1 -column 0 -sticky nsew
grid .misc.render_method -row 1 -column 1 -sticky nsew
grid .misc.render_args -row 2 -column 0 -sticky nsew
grid .misc.plugin_path -row 2 -column 1 -sticky nsew

pack .topic -expand 1 -fill both
pack .fractal -side left
pack .left.center_real -side top -expand 1 -fill both
pack .left.center_imaginary -side top -expand 1 -fill both
pack .left.scale -side top -expand 1 -fill both
pack .left.iterations -side top -expand 1 -fill both
pack .right.zoom -side top -expand 1 -fill both

pack .left -side top -expand 1 -fill both
pack .right -side top -expand 1 -fill both
pack .buttons -side top -expand 1 -fill both
pack .misc -side left -expand 1 -fill both


## Start parameter field
pack .start_params.fractal -fill both -expand 1
pack .start_params.fractal_parameter -fill both -expand 1
pack $fractal_parameter_win.entry -side left -fill both -expand 1
pack .start_params.output_method -fill both -expand 1
pack .start_params.output_parameter -fill both -expand 1
pack $output_parameter_win.entry -side left -fill both -expand 1
pack .start_params.render_method  -fill both -expand 1
pack .start_params.render_parameter -fill both -expand 1
pack $render_parameter_win.entry -side left -fill both -expand 1
#pack .start_params.geometry -fill both -expand 1
#pack $geometry_win.entry -side left -fill both -expand 1
pack .start_params.plugin_path -fill both -expand 1
pack $plugin_path_win.entry -side left -fill both -expand 1
pack $plugin_path_win.button -side left -fill both -expand 1
pack .start_params.precision -fill both -expand 1
pack $precision_win.entry -side left -fill both -expand 1

pack .start_params.buttons.start_tinyfract -side left -fill both -expand 1
pack .start_params.buttons.cancel -side left -fill both -expand 1
pack .start_params.buttons.load_parameter -side left -fill both -expand 1

pack .start_params.buttons -expand 1 -fill both

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
