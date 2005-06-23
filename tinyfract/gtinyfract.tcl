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
set plugin_path $env(HOME)/cvs/tinyfract/plugins
set output_method x11
set output_parameter "H10,.5S10,.5B10,.5"
set render_method "recurse"
set render_parameter "3"
set precision "100"
set scale "4"
set center "0,0"
set iteration_steps "100"
set fractal_args ""
set zoom_faktor 10

set TINYFRACT_FD 0

## Function for loading
proc load_options {} \
{
	global fractal plugin_path output_method output_parameter render_method render_parameter precision center scale iteration_stepsfractal_args

	set path [ tk_getOpenFile ]
	set path_fd [ open $path "r" ]
	set command [ gets $path_fd ]

	for "" "$command == -1" "set command [ gets $path_fd ]" "catch [ $command ]"
}
	

## Function for saving
proc safe_options {} \
{
	global fractal plugin_path output_method output_parameter render_method render_parameter precision fractal_args

	set path [ tk_getOpenFile ]
	set path_fd [ open $path "r+" ]
	
	set center [ .left.center.center get ]
	set scale [ .left.scale.scale get ]
	set iteration_steps [ .left.iterations.iteration_steps get ]

	puts $path_fd "set fractal $fractal\nset plugin_path $plugin_path\nset output_method $output_method\nset output_parameter $output_parameter\nset render_method $render_method\nset render_parameter $render_parameter\nset precision $precision\nset scale $scale\nset center $center\nset iteration_steps $iteration_steps\nset fractal_args $fractal_args\n"
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
proc new_args_cmd { center_real center_imaginary scale iteration_steps } \
{
	.left.center.center delete 0 end
	.left.iterations.iteration_steps delete 0 end
	.left.scale.scale delete 0 end

	.left.center.center insert 0 "$center_real,$center_imaginary"
	.left.iterations.iteration_steps insert 0 "$iteration_steps"
	.left.scale.scale insert 0 "$scale"
}

$parser alias new_args new_args_cmd


## Function for first rendering
proc first_rendering {} \
{
	global test TINYFRACT_FD geometry
	
	set fractal [ .params_for_tinyfract_starting.params.fractal get ]
	set fractal_parameter [ .params_for_tinyfract_starting.params.fractal_parameter get ]
	set plugin_path [ .params_for_tinyfract_starting.params.plugin_path get ]
	set output_method [ .params_for_tinyfract_starting.params.output_method get ]
	set output_parameter [ .params_for_tinyfract_starting.params.output_parameter get ]
	set render_method [ .params_for_tinyfract_starting.params.render_method get ]
	set render_parameter [ .params_for_tinyfract_starting.params.render_parameter get ]
	set precision [ .params_for_tinyfract_starting.params.precision get ]
	
	set help ""

	if ([regexp "xxx" $precision]==1) \
	{
		wm deiconify .error
		.error.message configure -text "You have to specify a precision"
		return
	}

	if ([regexp "xxx" $fractal]==1) \
	{
		wm deiconify .error
		.error.message configure -text "You have to specify a fractal type"
		return
	}

	if ([regexp "xxx" $geometry ]==1) \
	{
		wm deiconify .error
		.error.message configure -text "You have to specify a geometry"
		return
	}
	
	if { [regexp "xxx" $plugin_path] == 1 } \
	{
		if { [ exec ./existenv TINYFRACT_PLUGIN_PATH ] == 0 } \
		{
			set test 0
			wm deiconify .error
			.error.message configure -text "You did not specify a plugin path you can difiny a plugin path\nin the environmental variable TINYFRACT_PLUGIN_PATH or in the entry in this programm"
			tkwait variable test
			return
		}
		set plug_path ""
	}

	if ([regexp "xxx" $output_method ]==1) \
	{
		if { [ exec ./existenv TINYFRACT_OUTPUT_METHOD ] == 0 } \
		{
			set test 0
			wm deiconify .error
			.error.message configure -text "You did not specifiy an output method you can difine a output method\nin the environmental variable TINYFRACT_OUTPUT_METHOD or in the entry in this programm."
			tkwait variable test
			return
		}
		set out_meth ""
	}

	if ([regexp "xxx" $render_method]==1) \
	{
		if { [ exec ./existenv TINYFRACT_RENDER_METHOD ] == 0 } \
		{
			set test 0
			wm deiconify .error
			.error.message configure -text "You did not specifiy a render method you can difine a render method\nin the environmental variable TINYFRACT_RENDER_METHOD or in the entry in thos programm."
			tkwait variable test
			return
		}
		set ren_meth ""
	}

	if ([regexp "xxx" $render_parameter]==1) \
	{
		set test 0
		wm deiconify .error
		.error.message configure -text "WARNING!!!: You did not specify render parameters but som render functions need parameters!!!"
		tkwait variable test
		set $ren_param ""
	}
	
	if ([regexp "xxx" $output_parameter]==1) \
	{
		set test 0
		wm deiconify .error
		.error.message configure -text "WARNING!!!: You did not specify output parameters but some output functions need parameters!!!"
		tkwait variable test
		set $ren_param ""
	}

	## Call tinyfract with standard parameters.
	set TINYFRACT_FD [ open "|./tinyfract -f$fractal -g$geometry -P$plugin_path -o$output_method -O$output_parameter -r$render_method -R$render_parameter -p$precision" "r+" ]

	fileevent $TINYFRACT_FD readable { set line [ gets $TINYFRACT_FD ] ; if { [ catch [ $parser eval $line ] result ] != 0 } { puts $result } }

}

## Start rendering function
proc render { TINYFRACT_FD mode } \
{
	global zoom_faktor

	if { $TINYFRACT_FD == 0 } \
	{
		puts "Error: No Pipe"
		exit
	}
	set center [ .left.center.center get ]
	set scale [ .left.scale.scale get ]
	set iteration_steps [ .left.iterations.iteration_steps get ]


	puts $TINYFRACT_FD "p$center\ns$scale\ni$iteration_steps\nr\n"

	flush $TINYFRACT_FD
}

## Make Toplevel in witch first important parameters can be given

## Build the toplevel

toplevel .params_for_tinyfract_starting

## Build necessary entry and information fields

## Info labels
frame .params_for_tinyfract_starting.info

label .params_for_tinyfract_starting.info.fractal_info -text "Fractal type:"
label .params_for_tinyfract_starting.info.fratctal_parameter_info -text "Fractal parameter:"
label .params_for_tinyfract_starting.info.plugin_path_info -text "Plugin path:"
label .params_for_tinyfract_starting.info.output_method_info -text "Output method:"
label .params_for_tinyfract_starting.info.output_method_params_info -text "Output parameter:"
label .params_for_tinyfract_starting.info.render_method_info -text "Render method:"
label .params_for_tinyfract_starting.info.render_method_params_info -text "Render parameter:"
label .params_for_tinyfract_starting.info.precision -text "Precision:"

## Entries
frame .params_for_tinyfract_starting.params

entry .params_for_tinyfract_starting.params.fractal
entry .params_for_tinyfract_starting.params.fractal_parameter
entry .params_for_tinyfract_starting.params.plugin_path
entry .params_for_tinyfract_starting.params.output_method
entry .params_for_tinyfract_starting.params.output_parameter
entry .params_for_tinyfract_starting.params.render_method
entry .params_for_tinyfract_starting.params.render_parameter
entry .params_for_tinyfract_starting.params.precision

.params_for_tinyfract_starting.params.fractal insert 0 $fractal
.params_for_tinyfract_starting.params.fractal_parameter insert 0 $fractal_args
.params_for_tinyfract_starting.params.plugin_path insert 0 $plugin_path
.params_for_tinyfract_starting.params.output_method insert 0 $output_method
.params_for_tinyfract_starting.params.output_parameter insert 0 $output_parameter
.params_for_tinyfract_starting.params.render_method insert 0 $render_method
.params_for_tinyfract_starting.params.render_parameter insert 0 $render_parameter
.params_for_tinyfract_starting.params.precision insert 0 $precision


## Build Buttons
frame .params_for_tinyfract_starting.buttons

button .params_for_tinyfract_starting.buttons.start_tinyfract \
	-text "OK" \
	-command { first_rendering ; wm withdraw .params_for_tinyfract_starting }
button .params_for_tinyfract_starting.buttons.cancel \
	-text "Cancel" \
	-command "exit"
button .params_for_tinyfract_starting.buttons.load_parameter \
	-text "Load" \
	-command "load_options"



## Make labels and info fields only for center, scale and iteration steps
frame .left
frame .right

frame .topic
frame .left.center
frame .left.scale
frame .left.iterations
frame .right.zoom
frame .buttons


label .topic.headline \
	-text "Graphical user interface for tinyfract" \
	-font "-adobe-helvetica-bold-r-normal-*-24-*-*-*-*-*-iso8859-1" \
	-fg red

label .left.center.center_info -text "Center:"
label .left.iterations.iteration_steps_info -text "Iteration steps:"
label .left.scale.scale_info -text "Scale:"

entry .left.center.center
entry .left.iterations.iteration_steps
entry .left.scale.scale

.left.center.center insert 0 $center
.left.iterations.iteration_steps insert 0 $iteration_steps
.left.scale.scale insert 0 $scale


## Build the progress bar
iwidgets::feedback .buttons.progress \
	-labeltext "Progress"

## Build Safe and Cancel button
button .buttons.cancel \
	-text "Cancel" \
	-command {exit;puts $TINYFRACT_FD q;flush $TINYFRACT_FD;close $TINYFRACT_FD}
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



focus .params_for_tinyfract_starting.params.fractal


toplevel .error
label .error.bitmap -bitmap error
label .error.message -text "test"
button .error.exit -text "OK" -command "wm withdraw .error"
button .error.return -text "Cancel" -command {wm withdraw .error}
#bind .error.return <ButtonPress> {return}
bind .error.exit <ButtonPress> "set test 5"
bind .error.return <ButtonPress> "set test 5"

wm withdraw .error


grid .error.message -row 0 -column 1
grid .error.bitmap -row 0 -column 0
grid .error.exit -row 1 -column 1
grid .error.return -row 1 -column 0

pack .topic.headline -expand 1 -fill both

pack .left.center.center_info -side left -expand 1 -fill both
pack .left.center.center -side left -expand 1 -fill both

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
pack .left.center -side left -expand 1 -fill both
pack .left.scale -side left -expand 1 -fill both
pack .left.iterations -side left -expand 1 -fill both
pack .right.zoom -side left -expand 1 -fill both
pack .buttons -side bottom -expand 1 -fill both

pack .left -side left -expand 1 -fill both
pack .right -side right -expand 1 -fill both

pack .params_for_tinyfract_starting.info.fractal_info
pack .params_for_tinyfract_starting.info.fratctal_parameter_info
pack .params_for_tinyfract_starting.info.plugin_path_info
pack .params_for_tinyfract_starting.info.output_method_info
pack .params_for_tinyfract_starting.info.output_method_params_info
pack .params_for_tinyfract_starting.info.render_method_info
pack .params_for_tinyfract_starting.info.render_method_params_info
pack .params_for_tinyfract_starting.info.precision

pack .params_for_tinyfract_starting.params.fractal
pack .params_for_tinyfract_starting.params.fractal_parameter
pack .params_for_tinyfract_starting.params.plugin_path
pack .params_for_tinyfract_starting.params.output_method
pack .params_for_tinyfract_starting.params.output_parameter
pack .params_for_tinyfract_starting.params.render_method
pack .params_for_tinyfract_starting.params.render_parameter
pack .params_for_tinyfract_starting.params.precision

pack .params_for_tinyfract_starting.buttons.start_tinyfract
pack .params_for_tinyfract_starting.buttons.cancel
pack .params_for_tinyfract_starting.buttons.load_parameter


pack .params_for_tinyfract_starting.info -side left -expand 1
pack .params_for_tinyfract_starting.params -side left -expand 1
pack .params_for_tinyfract_starting.buttons -side top -expand 1


