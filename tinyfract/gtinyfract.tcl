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

proc render {} \
{
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

	exec "./tinyfract"
	#exec "./tinyfract -f$fractal --geom=$geom -P$plug_path -c$center -i$iter -o$out_meth -O$out_param -r$ren_meth -R$ren_param -s$scale"
}


button .buttons.cancel \
	-text "Cancel" \
	-command "exit"

button .buttons.render \
	-text "Render" \
	-command render




focus .param_first.fractal

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
