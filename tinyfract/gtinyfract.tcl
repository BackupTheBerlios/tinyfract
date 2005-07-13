#!/usr/bin/wish

## Include Iwidgets
package require Itcl
package require Itk
package require Iwidgets

wm title . "Tinyfract GUI"

## Create safe interpreter

set parser [ interp create -safe ]

## Standard parameters
set fractal mandelbrot
set geometry "[ expr [ winfo screenwidth . ] * 2 / 3 ]x[ expr [ winfo screenheight . ] * 2 / 3 ]"
set plugin_path "$env(HOME)/cvs/tinyfract/plugins"
set output_method x11
set output_parameter "H10,.5S10,.5B10,.5"
set render_method recurse
set render_method "recurse"
set render_parameter "3"
set precision "100"
set scale "4"
set scale_test 0
set center_real 0
set center_imaginary 0
set iteration_steps "100"
set fractal_args ""
set zoom_faktor 10

set TINYFRACT_FD 0

## Set the geometry for .
wm geometry . [ winfo screenwidth . ]x[winfo screenheight . ]

## Withdraw .
wm withdraw .

## Make nice colors for the interface
option add *Menubutton.background yellow 100
option add *Menubutton.foreground blue 100

option add *Label.background yellow 100
option add *Label.foreground blue 100

option add *Entry.background yellow 100
option add *Entry.foreground blue 100

option add *Button.background yellow 100
option add *Button.foreground blue 100

option add *Menu.background yellow 100
option add *Menu.foreground blue 100

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

## Functionds
## Function for inerting options
proc insert {} \
{
	global fractal fractal_args plugin_path output_method output_parameter render_method render_parameter precision center_real center_imaginary scale iteration_steps

	.start_params.fractal_parameter.params delete 0 end
	.start_params.output_parameter.params delete 0 end
	.start_params.render_parameter.params delete 0 end
	.start_params.prec.params delete 0 end

	.left.center_real.center_real delete 0 end
	.left.center_imaginary.center_imaginary delete 0 end
	.left.iterations.iteration_steps delete 0 end
	.left.scale.scale delete 0 end
	
	.start_params.fractal_parameter.params insert 0 $fractal_args
	.start_params.output_parameter.params insert 0 $output_parameter
	.start_params.render_parameter.params insert 0 $render_parameter
	.start_params.prec.params insert 0 $precision

	.left.center_real.center_real insert 0 $center_real
	.left.center_imaginary.center_imaginary insert 0 $center_imaginary
	.left.iterations.iteration_steps insert 0 $iteration_steps
	.left.scale.scale insert 0 $scale

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

## Function for loading
proc load_options {} \
{
	global parser

	set path [ tk_getOpenFile ]
	set path_fd [ open $path "r" ]

	for {set line [ gets $path_fd ]} {![eof $path_fd]} {set line [ gets $path_fd ]} { $parser eval $line }
	insert
}

## Function for saving
proc safe_options {} \
{
	global fractal plugin_path output_method output_parameter render_method render_parameter precision fractal_args

	set path [ tk_getOpenFile ]
	set path_fd [ open $path "r+" ]
	
	set center_real [ .left.center_real.center_real get ]
	set center_imaginary [ .left.center_imaginary.center_imaginary get ]
	set scale [ .left.scale.scale get ]
	set iteration_steps [ .left.iterations.iteration_steps get ]

	puts $path_fd "define fractal {$fractal}\ndefine plugin_path {$plugin_path}\ndefine output_method {$output_method}\ndefine output_parameter {$output_parameter}\ndefine render_method {$render_method}\ndefine render_parameter {$render_parameter}\ndefine precision {$precision}\ndefine scale {$scale}\ndefine center_real {$center_real}\ndefine center_imaginary {$center_imaginary}\ndefine iteration_steps {$iteration_steps}\ndefine fractal_args {$fractal_args}\n"
}

## Function for making a progress bar
proc progress_cmd { actual total } \
{
	if { $actual == 1 } \
	{
		.buttons.progress reset
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
	return
}
$parser alias scale scale

## Function which evals options
proc eventdata { TINYFRACT_FD } \
{
	global parser
	if {![ eof $TINYFRACT_FD ] } \
	{
		set line [ gets $TINYFRACT_FD ]
		if { [ catch [ $parser eval $line ] result ] != 0 } { puts "While executing ($line) this error occured:($result)" }	
	}
}

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
	global zoom_faktor scale_test


	if { $TINYFRACT_FD == 0 } \
	{
		puts "Error: No Pipe"
		exit
	}
	puts "I am living $scale_test"

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
	global test TINYFRACT_FD geometry fractal plugin_path output_method render_method

	set fractal_parameter [ .start_params.fractal_parameter.params get ]
	set output_parameter [ .start_params.output_parameter.params get ]
	set render_parameter [ .start_params.render_parameter.params get ]
	set precision [ .start_params.prec.params get ]
	
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
	if { $fractal == "mandelbrot" } \
	{
		set TINYFRACT_FD [ open "|./tinyfract -f$fractal -g$geometry -P$plugin_path -o$output_method -O$output_parameter -r$render_method -R$render_parameter -p$precision" "r+" ]
		puts "./tinyfract -f$fractal -g$geometry -P$plugin_path -o$output_method -O$output_parameter -r$render_method -R$render_parameter -p$precision"
	} else \
	{
		set TINYFRACT_FD [ open "|./tinyfract -f$fractal -F$fractal_parameter -g$geometry -P$plugin_path -o$output_method -O$output_parameter -r$render_method -R$render_parameter -p$precision" "r+" ]
		puts "./tinyfract -f$fractal -F$fractal_parameter -g$geometry -P$plugin_path -o$output_method -O$output_parameter -r$render_method -R$render_parameter -p$precision"
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
## Helper frames
frame .start_params.fractal
frame .start_params.fractal_parameter
frame .start_params.plugin_path
frame .start_params.output_method
frame .start_params.output_parameter
frame .start_params.render_method
frame .start_params.render_parameter
frame .start_params.prec

## Info labels
label .start_params.fractal.info -text "Fractal type:"
label .start_params.fractal_parameter.info -text "Fractal parameter:"
label .start_params.plugin_path.info -text "Plugin path:"
label .start_params.output_method.info -text "Output method:"
label .start_params.output_parameter.info -text "Output parameter:"
label .start_params.render_method.info -text "Render method:"
label .start_params.render_parameter.info -text "Render parameter:"
label .start_params.prec.info -text "Precision:"

## Entries or Menubuttons for prameters insert
menubutton .start_params.fractal.params \
	-text $fractal \
	-activebackground darkgrey
entry .start_params.fractal_parameter.params
label .start_params.plugin_path.params \
	-text $plugin_path \
	-background red
menubutton .start_params.output_method.params \
	-text $output_method \
	-activebackground darkgrey
entry .start_params.output_parameter.params
menubutton .start_params.render_method.params \
	-text $render_method \
	-activebackground darkgrey
entry .start_params.render_parameter.params
entry .start_params.prec.params


## Create menues and other volataile widgets
## Fractal type
menu .start_params.fractal.params.menu
.start_params.fractal.params.menu add command -label "Bitte Wählen" -command { .start_params.fractal.params configure -text "Bitte wählen" ; set fractal "" }
.start_params.fractal.params.menu add command -label "Mandelbrot" -command { .start_params.fractal.params configure -text "Mandelbrot" ; set fractal mandelbrot }
.start_params.fractal.params.menu add command -label "Julia" -command { .start_params.fractal.params configure -text "Julia" ; set fractal julia }
.start_params.fractal.params.menu add command -label "Lambda" -command { .start_params.fractal.params configure -text "Lambda" ; set fractal lambda }
.start_params.fractal.params configure -menu ".start_params.fractal.params.menu"

## Plugin Path
button .start_params.plugin_path.search \
	-text "Durchsuchen" \
	-command { if { [ .file activate ] } { set plugin_path [ .file get ] ; .start_params.plugin_path.params configure -text $plugin_path } }

## Output method
menu .start_params.output_method.params.menu
.start_params.output_method.params.menu add command -label "Bitte Wählen" -command { .start_params.output_method.params configure -text "Bitte wählen" ; set output_method "" }
.start_params.output_method.params.menu add command -label "X11" -command { .start_params.output_method.params configure -text "x11" ; set output_method mandelbrot }
.start_params.output_method.params.menu add command -label "aa" -command { .start_params.output_method.params configure -text "aa" ; set output_method julia }
.start_params.output_method.params configure -menu ".start_params.output_method.params.menu"

## Render method
menu .start_params.render_method.params.menu
.start_params.render_method.params.menu add command -label "Bitte Wählen" -command { .start_params.render_method.params configure -text "Bitte wählen" ; set render_method "" }
.start_params.render_method.params.menu add command -label "Recurse" -command { .start_params.render_method.params configure -text "Recurse" ; set render_method recurse }
.start_params.render_method.params.menu add command -label "dumb" -command { .start_params.render_method.params configure -text "dumb" ; set render_method dumb }
.start_params.render_method.params.menu add command -label "pix" -command { .start_params.render_method.params configure -text "pix" ; set render_method pix }
.start_params.render_method.params configure -menu ".start_params.render_method.params.menu"


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
	-font "-adobe-helvetica-bold-r-normal-*-24-*-*-*-*-*-iso8859-1" \
	-fg red

label .left.center_real.center_real_info -text "Center(Real part):"
label .left.center_imaginary.center_imaginary_info -text "Center(Imaginary part):"
label .left.iterations.iteration_steps_info -text "Iteration steps:"
label .left.scale.scale_info -text "Scale:"

entry .left.center_real.center_real
entry .left.center_imaginary.center_imaginary
entry .left.iterations.iteration_steps
entry .left.scale.scale

## Insert options
insert


## Build the progress bar
iwidgets::feedback .buttons.progress \
	-labeltext "Progress"

## Build Safe and Cancel button
button .buttons.cancel \
	-text "Cancel" \
	-command { puts $TINYFRACT_FD q ; flush $TINYFRACT_FD ; close $TINYFRACT_FD ; exit }
button .buttons.safe \
	-text safe \
	-command "safe_options"

## Build Buttons for auto zooming
label .right.zoom.faktor \
	-text $zoom_faktor
button .right.zoom.zoom_in \
	-text "+" \
	-command { render $TINYFRACT_FD 1 }
button .right.zoom.zoom_out \
	-text "-" \
	-command { render $TINYFRACT_FD 0 }



focus .start_params.fractal.params


toplevel .error
label .error.bitmap -bitmap error
label .error.message -text "test"
button .error.exit -text "OK" -command "wm withdraw .error"
button .error.return -text "Cancel" -command {wm withdraw .error}
#bind .error.return <ButtonPress> {return}
bind .error.exit <ButtonPress> "set test 5"
bind .error.return <ButtonPress> "set test 5"

wm withdraw .error


grid .error.message -row 0 -column 1 -sticky nsew
grid .error.bitmap -row 0 -column 0 -sticky nsew
grid .error.exit -row 1 -column 1 -sticky nsew
grid .error.return -row 1 -column 0 -sticky nsew

pack .topic.headline -expand 1 -fill both

pack .left.center_real.center_real_info -side left -expand 1 -fill both
pack .left.center_imaginary.center_imaginary_info -side left -expand 1 -fill both
pack .left.center_real.center_real -side left -expand 1 -fill both
pack .left.center_imaginary.center_imaginary -side left -expand 1 -fill both

pack .left.iterations.iteration_steps_info -side left -expand 1 -fill both
pack .left.iterations.iteration_steps -side left -expand 1 -fill both

pack .left.scale.scale_info -side left -expand 1 -fill both
pack .left.scale.scale -side left -expand 1 -fill both

pack .right.zoom.faktor -expand 1 -fill both
pack .right.zoom.zoom_in -side left -expand 1 -fill both
pack .right.zoom.zoom_out -side left -expand 1 -fill both

pack .buttons.cancel -expand 1 -fill both
pack .buttons.safe -expand 1 -fill both
pack .buttons.progress -expand 1 -fill both

pack .topic -expand 1 -fill both
pack .left.center_real -side left -expand 1 -fill both
pack .left.center_imaginary -side left -expand 1 -fill both
pack .left.scale -side left -expand 1 -fill both
pack .left.iterations -side left -expand 1 -fill both
pack .right.zoom -side left -expand 1 -fill both
pack .buttons -side bottom -expand 1 -fill both

pack .left -side left -expand 1 -fill both
pack .right -side right -expand 1 -fill both

pack .start_params.fractal.info -side left -fill both -expand 1
pack .start_params.fractal_parameter.info -side left -fill both -expand 1
pack .start_params.plugin_path.info -side left -fill both -expand 1
pack .start_params.output_method.info -side left -fill both -expand 1
pack .start_params.output_parameter.info -side left -fill both -expand 1
pack .start_params.render_method.info -side left -fill both -expand 1
pack .start_params.render_parameter.info -side left -fil both -expand 1
pack .start_params.prec.info -side left -fill both -expand 1

pack .start_params.fractal.params -side left -fill both -expand 1
pack .start_params.fractal_parameter.params -side left -fill both -expand 1
pack .start_params.plugin_path.params -side left -fill both -expand 1
pack .start_params.plugin_path.search -side left -fill both -expand 1
pack .start_params.output_method.params -side left -fill both -expand 1
pack .start_params.output_parameter.params -side left -fill both -expand 1
pack .start_params.render_method.params -side left -fill both -expand 1
pack .start_params.render_parameter.params -side left -fill both -expand 1
pack .start_params.prec.params -side left -fill both -expand 1

pack .start_params.buttons.start_tinyfract -side left -fill both -expand 1
pack .start_params.buttons.cancel -side left -fill both -expand 1
pack .start_params.buttons.load_parameter -side left -fill both -expand 1


pack .start_params.fractal -expand 1 -fill both
pack .start_params.fractal_parameter -expand 1 -fill both
pack .start_params.plugin_path -expand 1 -fill both
pack .start_params.output_method -expand 1 -fill both
pack .start_params.output_parameter -expand 1 -fill both
pack .start_params.render_method -expand 1 -fill both
pack .start_params.render_parameter -expand 1 -fill both
pack .start_params.prec -expand 1 -fill both

pack .start_params.buttons -expand 1 -fill both


