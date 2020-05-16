include <case.scad>;
include <_quality.scad>;

module case_buttons_volume() {
    $x_vol_up = $case_l - $offset_inner - 0.9 * $button_d;
    $x_vol_down = $x_vol_up - 1.4 * $button_d;
    $center = $case_l / 2;
    $gap = 7;
    
    difference() {
        union() {
            case();
            translate([$x_vol_up, $center, $wall/2]) cylinder(d=$button_d + $wall, h=$wall/2+2);
            translate([$x_vol_down, $center, $wall/2]) cylinder(d=$button_d + $wall, h=$wall/2+2);
            translate([$center, $center, $wall]) rotate([90,0,45])
                cylinder(d=2*$wall, h=$inner_diagon, center=true);
            translate([$center, $center, $wall]) rotate([90,0,-45])
                cylinder(d=2*$wall, h=$inner_diagon, center=true);
            translate([$x_vol_up, $center + $button_d, $wall/2]) cylinder(d=$button_d*0.6, h=$wall);
            translate([$x_vol_down, $center + $button_d, $wall/2]) cylinder(d=$button_d*0.6, h=$wall);
            translate([$center, $case_l*0.75, $wall]) rotate([0,90,0])
                cylinder(d=1.4*$wall, h=$case_l*0.5, center=true);
            translate([$center, $case_l*0.25, $wall]) rotate([0,90,0])
                cylinder(d=1.4*$wall, h=$case_l*0.5, center=true);
            translate([$case_l*0.75, $center, $wall]) rotate([90,0,0])
                cylinder(d=1.4*$wall, h=$case_l*0.5, center=true);
            translate([$case_l*0.25, $center, $wall]) rotate([90,0,0])
                cylinder(d=1.4*$wall, h=$case_l*0.5, center=true);
        };
        translate([$x_vol_up, $center, -1]) cylinder(d=$button_d, h=$wall+4);
        translate([$x_vol_down, $center, -1]) cylinder(d=$button_d, h=$wall+4);
        translate([($x_vol_up + $x_vol_down)/2, $center, 1.5 * $wall + 1.2])
            cube([2.4 * $button_d + 2*$wall, $gap, $wall+2], center=true);
        
        translate([$x_vol_up, $center + $button_d, 0]) {
            cube([$button_d * 0.5, $button_d * 0.1, 1.5*$wall], center=true);
            cube([$button_d * 0.1, $button_d * 0.5, 1.5*$wall], center=true);
        }
        translate([$x_vol_down, $center + $button_d, 0]) {
            cube([$button_d * 0.1, $button_d * 0.5, 1.5*$wall], center=true);
        }
    };
};

case_buttons_volume();