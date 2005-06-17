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
set geometry "500x500"
set plugin_path $env(HOME)/cvs/tinyfract/plugins
set output_method x11
set output_parameter "H10,.5B10,.5S10,.5"
set render_method recurse
set render_parameter 3
set precision 20
set scale 4
set center 0,0
set iteration_steps 400
set fractal_args ""

set TINYFRACT_FD 0

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
	.param_first.center delete 0 end
	.param_first.iteration_steps delete 0 end
	.param_second.scale delete 0 end

	.param_first.center insert 0 "$center_real,$center_imaginary"
	.param_first.iteration_steps insert 0 "$iteration_steps"
	.param_second.scale insert 0 "$scale"
}

$parser alias new_args new_args_cmd


## Function for first rendering
proc first_rendering {} \
{
	global test TINYFRACT_FD
	
	set fractal [ .param_first.fractal get ]
	set geometry [ .param_first.geom get ]
	set plugin_path [ .param_first.plug_path get ]
	set output_method [ .param_second.out_meth get ]
	set output_parameter [ .param_second.out_param get ]
	set render_method [ .param_second.ren_meth get ]
	set render_parameter [ .param_second.ren_param get ]
	set precision [ .param_second.precision get ]
	set fractal_args [ .param_first.fractal_args get ]
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


	## Disable and enable entries
	.param_first.fractal configure \
		-state disabled
	.param_first.geom configure \
		-state disabled
	.param_first.plug_path configure \
		-state disabled
	.param_first.center configure \
		-state normal
	.param_first.iteration_steps configure \
		-state normal
	.param_first.fractal_args configure \
		-state disabled

	.param_second.out_meth configure \
		-state disabled
	.param_second.out_param configure \
		-state disabled
	.param_second.ren_meth configure \
		-state disabled
	.param_second.ren_param configure \
		-state disabled
	.param_second.scale configure \
		-state normal
	.param_second.precision configure \
		-state disabled

	## Call tinyfract with standard parameters.
	set TINYFRACT_FD [ open "|./tinyfract -f$fractal -g$geometry -P$plugin_path -o$output_method -O$output_parameter -r$render_method -R$render_parameter -p$precision" "r+" ]

	fileevent $TINYFRACT_FD readable { set line [ gets $TINYFRACT_FD ] ; if { [ catch [ $parser eval $line ] result ] != 0 } { puts $result } }

}





## Start rendering function
proc render { TINYFRACT_FD } \
{
	set center [ .param_first.center get ]
	set scale [ .param_second.scale get ]
	set iteration_steps [ .param_first.iteration_steps get ]

	puts $TINYFRACT_FD "p$center\ns$scale\ni$iteration_steps\nr\n"

	flush $TINYFRACT_FD
}

## Make labels and info fields
frame .topic -bg blue 
frame .info_first -bg red
frame .info_second -bg yellow
frame .param_first -bg black
frame .param_second -bg green
frame .buttons -bg pink 


label .topic.headline \
	-text "Graphical user interface for tinyfract" \
	-font "-adobe-helvetica-bold-r-normal-*-24-*-*-*-*-*-iso8859-1" \
	-fg red

label .info_first.fractal -text "Fractal type:"
label .info_first.geom -text "Geometry(your screen: [ winfo screenwidth . ]x[winfo screenheight . ]):"
label .info_first.plug_path -text "Plugin path:"
label .info_first.center -text "Center:"
label .info_first.iteration_steps -text "Iteration steps:"
label .info_first.fractal_args -text "Fractal Parameter:"

label .info_second.out_meth -text "Output method:"
label .info_second.out_param -text "Output method parameters:"
label .info_second.ren_meth -text "Render method:"
label .info_second.ren_param -text "Render method parameters:"
label .info_second.scale -text "Scale:"
label .info_second.precision -text "Precision:"

entry .param_first.fractal
entry .param_first.geom
entry .param_first.plug_path
entry .param_first.center \
	-state disabled
entry .param_first.iteration_steps \
	-state disabled
entry .param_first.fractal_args

entry .param_second.out_meth
entry .param_second.out_param 
entry .param_second.ren_meth
entry .param_second.ren_param
entry .param_second.scale \
	-state disabled
entry .param_second.precision

iwidgets::feedback .buttons.progress \
	-labeltext "Progress"

.param_first.fractal insert 0 $fractal
.param_first.geom insert 0 $geometry
.param_first.plug_path insert 0 $plugin_path
.param_first.center insert 0 $center
.param_first.iteration_steps insert 0 $iteration_steps
.param_first.fractal_args insert 0 $fractal_args
.param_second.out_meth insert 0 $output_method
.param_second.out_param insert 0 $output_parameter
.param_second.ren_meth insert 0 $render_method
.param_second.ren_param insert 0 $render_parameter
.param_second.scale insert 0 $scale
.param_second.precision insert 0 $precision

toplevel .error
label .error.bitmap -bitmap error
label .error.message -text "test"
button .error.exit -text "OK" -command "wm withdraw .error"
button .error.return -text "Cancel" -command {wm withdraw .error}
#bind .error.return <ButtonPress> {return}
bind .error.exit <ButtonPress> "set test 5"
bind .error.return <ButtonPress> "set test 5"

wm withdraw .error



button .buttons.cancel \
	-text "Cancel" \
	-command {exit;puts $TINYFRACT_FD q;flush $TINYFRACT_FD;close $TINYFRACT_FD}

button .buttons.render \
	-text "Render" \
	-command { render $TINYFRACT_FD }

button .buttons.first_render \
	-text "Start tinyfract" \
	-command "first_rendering"




focus .param_first.fractal


grid .error.message -row 0 -column 1
grid .error.bitmap -row 0 -column 0
grid .error.exit -row 1 -column 1
grid .error.return -row 1 -column 0

pack .topic.headline -expand 1 -fill both

pack .info_first.fractal -expand 1 -fill both
pack .info_first.geom -expand 1 -fill both
pack .info_first.plug_path -expand 1 -fill both
pack .info_first.center -expand 1 -fill both
pack .info_first.iteration_steps -expand 1 -fill both
pack .info_first.fractal_args -expand 1 -fill both

pack .info_second.out_meth -expand 1 -fill both
pack .info_second.out_param -expand 1 -fill both
pack .info_second.ren_meth -expand 1 -fill both
pack .info_second.ren_param -expand 1 -fill both
pack .info_second.scale -expand 1 -fill both
pack .info_second.precision -expand 1 -fill both

pack .param_first.fractal -expand 1 -fill both
pack .param_first.geom -expand 1 -fill both
pack .param_first.plug_path -expand 1 -fill both
pack .param_first.center -expand 1 -fill both
pack .param_first.iteration_steps -expand 1 -fill both
pack .param_first.fractal_args -expand 1 -fill both

pack .param_second.out_meth -expand 1 -fill both
pack .param_second.out_param -expand 1 -fill both
pack .param_second.ren_meth -expand 1 -fill both
pack .param_second.ren_param -expand 1 -fill both
pack .param_second.scale -expand 1 -fill both
pack .param_second.precision -expand 1 -fill both

pack .buttons.cancel -expand 1 -fill both
pack .buttons.render -expand 1 -fill both
pack .buttons.first_render -expand 1 -fill both
pack .buttons.progress -expand 1 -fill both


pack .topic -expand 1 -fill both
pack .buttons -side bottom -expand 1 -fill both
pack .info_first -side left -expand 1 -fill both
pack .param_first -side left -expand 1 -fill both
pack .info_second -side left -expand 1 -fill both
pack .param_second -side left -expand 1 -fill both


