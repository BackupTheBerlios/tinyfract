#!/usr/bin/wish

wm title . "graphic tinyfract"






frame .topic -bg blue 
frame .info_first -bg red
frame .info_second -bg yellow
frame .param_first -bg black
frame .param_second -bg green
frame .buttons -bg pink 


label .topic.headline \
	-text "Graphical Oberfläche for tinyfract" \
	-font "-adobe-heveltica-bold-r-normal-*-24-*-*-*-*-*-iso8859-1" \
	-fg red

label .info_first.fractal -text "Fractal type:"
label .info_first.geom -text "Geometry(your screen: [ winfo screenwidth . ]x[winfo screenheight . ]):"
label .info_first.plug_path -text "Plugin path:"
label .info_first.center -text "Center:"
label .info_first.iter -text "Iteration steps:"

label .info_second.out_meth -text "Output method:"
label .info_second.out_param -text "Output method parameters:"
label .info_second.ren_meth -text "Render method:"
label .info_second.ren_param -text "Render method parameters:"
label .info_second.scale -text "Scale:"

entry .param_first.fractal
entry .param_first.geom
entry .param_first.plug_path
entry .param_first.center
entry .param_first.iter

entry .param_second.out_meth
entry .param_second.out_param 
entry .param_second.ren_meth
entry .param_second.ren_param
entry .param_second.scale

.param_first.fractal insert 0 "xxx"
.param_first.geom insert 0 "xxx"
.param_first.plug_path insert 0 "xxx"
.param_first.center insert 0 "xxx"
.param_first.iter insert 0 "xxx"
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
	set iter [ .param_first.iter get ]
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

	if ([regexp "xxx" $plug_path]==1) \
	{
		set test 0
		wm deiconify .error
		.error.message configure -text "You did not specified a plugin path but if you have difined a plugin path\nin the environmental variable TINYFRACT_PLUGIN_PATH it is no problem"
		tkwait variable test
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
		set center "-c$help"
	}
	if ([regexp "xxx" $iter]==1) \
	{
		set iter ""
	} else \
	{
		set help $iter
		set iter "-i$help"
	}

	if ([regexp "xxx" $out_meth]==1) \
	{
		set test 0
		wm deiconify .error
		.error.message configure -text "You did not specified an output method but if you have difined a output method\nin the environmental variable TINYFRACT_OUTPUT_METHOD it is no problem"
		tkwait variable test
		set out_meth ""
	} else \
	{
		set help $out_meth
		set out_meth "-o$help"
	}

	if ([regexp "xxx" $ren_meth]==1) \
	{
		set test 0
		wm deiconify .error
		.error.message configure -text "You did not specified a render method but if you have difined a render method\nin the environmental variable TINYFRACT_RENDER_METHOD it is no problem"
		tkwait variable test
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
		set scale "-s$help"
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
		puts hallo
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

	if { [ catch { exec ./tinyfract $fractal $geom $plug_path $center $iter $out_meth $out_param $ren_meth $ren_param $scale } error_message ] == 1 } \
	{
		toplevel .stdout
		wm title .stdout "Message"
		text .stdout.error -xscrollcommand ".stdout.scroll set"
		scrollbar .stdout.scroll -orient vertical -command { .stdout.error yview }
		button .stdout.exit -text "OK" -command "destroy .stdout"
		.stdout.error insert end "Following message was given:\n\n$error_message\n\nIf You dont know waht the error means write an E-Mail to the programmers of this\nprogramm:\nJan Kandziora <jjj@gmx.de>\nAlexander Kreiss <AKreiss@gmx.de>\n"
		pack .stdout.scroll -side right -fill y -expand 1
		pack .stdout.error -expand 1 -fill both
		pack .stdout.exit -expand 1 -fill both
	}
}


button .buttons.cancel \
	-text "Cancel" \
	-command "exit"

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
pack .info_first.iter -expand 1 -fill both

pack .info_second.out_meth -expand 1 -fill both
pack .info_second.out_param -expand 1 -fill both
pack .info_second.ren_meth -expand 1 -fill both
pack .info_second.ren_param -expand 1 -fill both
pack .info_second.scale -expand 1 -fill both

pack .param_first.fractal -expand 1 -fill both
pack .param_first.geom -expand 1 -fill both
pack .param_first.plug_path -expand 1 -fill both
pack .param_first.center -expand 1 -fill both
pack .param_first.iter -expand 1 -fill both

pack .param_second.out_meth -expand 1 -fill both
pack .param_second.out_param -expand 1 -fill both
pack .param_second.ren_meth -expand 1 -fill both
pack .param_second.ren_param -expand 1 -fill both
pack .param_second.scale -expand 1 -fill both

pack .buttons.cancel -expand 1 -fill both -side left
pack .buttons.render -expand 1 -fill both -side left

pack .topic -expand 1 -fill both
pack .buttons -side bottom -expand 1 -fill both
pack .info_first -side left -expand 1 -fill both
pack .param_first -side left -expand 1 -fill both
pack .info_second -side left -expand 1 -fill both
pack .param_second -side left -expand 1 -fill both
