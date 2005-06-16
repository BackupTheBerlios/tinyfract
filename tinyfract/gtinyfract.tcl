#!/usr/bin/wish

## Include Iwidgets
package require Itcl
package require Itk
package require Iwidgets

wm title . "Tinyfract GUI"

## Standard parameters.
set fractal mandelbrot
set geometry "500x500"
set plugin_path $env(HOME)/cvs/tinyfract/plugins
set output_method x11
set output_parameter "H10,.5B10,.5S10,.5"
set render_method recurse
set render_parameter 3
set precision 20


## Function for making a progress bar
proc progress_bar { actual total } \
{
	if { $actual == 0 } \
	{
		.buttons.progress reset
	}
	.buttons.progress configure -steps $total
	.buttons.progress step

}


## Function for status parsing
proc parse_status { message } \
{
	if { [ scan $message "progress %d %d" actual total ] == 2 } \
	{
		progress_bar $actual $total
	}
}





## Call tinyfract with standard parameters.
set TINYFRACT_FD [ open "|./tinyfract -f$fractal -g$geometry -P$plugin_path -o$output_method -O$output_parameter -r$render_method -R$render_parameter -p$precision" "r+" ]


puts $TINYFRACT_FD "p0,0\ns4\ni1000\nr\n"

flush $TINYFRACT_FD

fileevent $TINYFRACT_FD readable { set line [ gets $TINYFRACT_FD ] ; puts $line ; parse_status $line }





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

label .info_second.out_meth -text "Output method:"
label .info_second.out_param -text "Output method parameters:"
label .info_second.ren_meth -text "Render method:"
label .info_second.ren_param -text "Render method parameters:"
label .info_second.scale -text "Scale:"

entry .param_first.fractal
entry .param_first.geom
entry .param_first.plug_path
entry .param_first.center
entry .param_first.iteration_steps

entry .param_second.out_meth
entry .param_second.out_param 
entry .param_second.ren_meth
entry .param_second.ren_param
entry .param_second.scale

iwidgets::feedback .buttons.progress \
	-labeltext "Progress"

.param_first.fractal insert 0 "xxx"
.param_first.geom insert 0 "xxx"
.param_first.plug_path insert 0 "xxx"
.param_first.center insert 0 "xxx"
.param_first.iteration_steps insert 0 "xxx"
.param_second.out_meth insert 0 "xxx"
.param_second.out_param insert 0 "xxx"
.param_second.ren_meth insert 0 "xxx"
.param_second.ren_param insert 0 "xxx"
.param_second.scale insert 0 "xxx"

toplevel .error
label .error.bitmap -bitmap error
label .error.message -text "test"
button .error.exit -text "OK" -command "wm withdraw .error"
button .error.return -text "Cancel" -command {wm withdraw .error}
#bind .error.return <ButtonPress> {return}
bind .error.exit <ButtonPress> "set test 5"
bind .error.return <ButtonPress> "set test 5"

wm withdraw .error


proc render {} \
{
	global test
	
	set fractal [ .param_first.fractal get ]
	set geom [ .param_first.geom get ]
	set plug_path [ .param_first.plug_path get ]
	set center [ .param_first.center get ]
	set iteration_steps [ .param_first.iteration_steps get ]
	set out_meth [ .param_second.out_meth get ]
	set out_param [ .param_second.out_param get ]
	set ren_meth [ .param_second.ren_meth get ]
	set ren_param [ .param_second.ren_param get ]
	set scale [ .param_second.scale get ]
	set help " "

	if ([regexp "xxx" $fractal]==1) \
	{
		wm deiconify .error
		.error.message configure -text "You have to specify a fractal type"
		return
	} else \
	{
		set help $fractal
		set fractal "-f$help"
	}

	if ([regexp "xxx" $geom]==1) \
	{
		wm deiconify .error
		.error.message configure -text "You have to specify a geometry"
		return
	} else \
	{
		set help $geom
		set geom "--geom=$help"
	}

	if { [regexp "xxx" $plug_path] == 1 } \
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
	} else \
	{
		set help $plug_path
		set plug_path "-P$help"
	}

	if ([regexp "xxx" $center]==1) \
	{
		set center ""
	} else \
	{
		set help $center
		set center "-$help"
	}
	if ([regexp "xxx" $iteration_steps]==1) \
	{
		set iteration_steps ""
	} else \
	{
		set help $iteration_steps
		set iteration_steps "-$help"
	}

	if ([regexp "xxx" $out_meth]==1) \
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
	} else \
	{
		set help $out_meth
		set out_meth "-o$help"
	}

	if ([regexp "xxx" $ren_meth]==1) \
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
	} else \
	{
		set help $ren_meth
		set ren_meth "-r$help"
	}

	if ([regexp "xxx" $scale]==1) \
	{
		set scale " "
	} else \
	{
		set help $scale
		set scale "$help"
	}

	if ([regexp "xxx" $ren_param]==1) \
	{
		set test 0
		wm deiconify .error
		.error.message configure -text "WARNING!!!: You did not specify render parameters but som render functions need parameters!!!"
		tkwait variable test
		set $ren_param ""
	} else \
	{
		set help $ren_param
		set ren_param "-R$help"
	}

	if ([regexp "xxx" $out_param]==1) \
	{
		set test 0
		wm deiconify .error
		.error.message configure -text "WARNING!!!: You did not specify output parameters but some output functions need parameters!!!"
		tkwait variable test
		set $ren_param ""
	} else \
	{
		set help $out_param
		set out_param "-O$help"
	}

	puts $TINYFRACT_FD "p$center\ns$scale\ni$iteration_stepsation_steps\nr\n"
}


button .buttons.cancel \
	-text "Cancel" \
	-command {exit;puts $TINYFRACT_FD q;flush $TINYFRACT_FD;close $TINYFRACT_FD}

button .buttons.render \
	-text "Render" \
	-command render




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

pack .info_second.out_meth -expand 1 -fill both
pack .info_second.out_param -expand 1 -fill both
pack .info_second.ren_meth -expand 1 -fill both
pack .info_second.ren_param -expand 1 -fill both
pack .info_second.scale -expand 1 -fill both

pack .param_first.fractal -expand 1 -fill both
pack .param_first.geom -expand 1 -fill both
pack .param_first.plug_path -expand 1 -fill both
pack .param_first.center -expand 1 -fill both
pack .param_first.iteration_steps -expand 1 -fill both

pack .param_second.out_meth -expand 1 -fill both
pack .param_second.out_param -expand 1 -fill both
pack .param_second.ren_meth -expand 1 -fill both
pack .param_second.ren_param -expand 1 -fill both
pack .param_second.scale -expand 1 -fill both

pack .buttons.cancel -expand 1 -fill both
pack .buttons.render -expand 1 -fill both
pack .buttons.progress -expand 1 -fill both


pack .topic -expand 1 -fill both
pack .buttons -side bottom -expand 1 -fill both
pack .info_first -side left -expand 1 -fill both
pack .param_first -side left -expand 1 -fill both
pack .info_second -side left -expand 1 -fill both
pack .param_second -side left -expand 1 -fill both


