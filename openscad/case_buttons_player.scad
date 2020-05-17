include <case.scad>;
include <_quality.scad>;

module case_buttons_volume() {
    module fr(left=true) {
        $x1 = -$button_d * 0.2;
        $x2 = $button_d * 0.2;
        $y1 = -$button_d * 0.12;
        $y2 = $button_d * 0.12;
        points = left ? [[$x1,$y2], [0,$y1], [$x2,$y2]] : [[$x1,$y1], [$x2,$y1], [0,$y2]];
        
        linear_extrude(height=1.5*$wall, center=true) {
            translate([0, -$button_d * 0.1, 0]) polygon(points);
            translate([0, $button_d * 0.1, 0]) polygon(points);
        };
    };

    $x_pause = $case_l - $offset_inner - 0.9 * $button_d;
    $x_fr = $x_pause - 0.2 * $button_d;
    $x_d_fr = $button_d * 1.3;
    $center = $case_l / 2;
    $gap = 7;
    
    difference() {
        union() {
            case();
            translate([$x_pause, $center, $wall/2]) cylinder(d=$button_d + $wall, h=$wall/2+2);
            translate([$x_fr, $center + $x_d_fr, $wall/2]) cylinder(d=$button_d + $wall, h=$wall/2+2);
            translate([$x_fr, $center - $x_d_fr, $wall/2]) cylinder(d=$button_d + $wall, h=$wall/2+2);
            translate([$center, $center, $wall]) rotate([90,0,45])
                cylinder(d=2*$wall, h=$inner_diagon, center=true);
            translate([$center, $center, $wall]) rotate([90,0,-45])
                cylinder(d=2*$wall, h=$inner_diagon, center=true);
            translate([$x_pause - $button_d * 0.9, $center, $wall/2]) cylinder(d=$button_d*0.6, h=$wall);
            translate([$x_fr - $button_d * 0.9, $center + $x_d_fr, $wall/2]) cylinder(d=$button_d*0.6, h=$wall);
            translate([$x_fr - $button_d * 0.9, $center - $x_d_fr, $wall/2]) cylinder(d=$button_d*0.6, h=$wall);
            translate([$center, $case_l*0.75, $wall]) rotate([0,90,0])
                cylinder(d=1.4*$wall, h=$case_l*0.5, center=true);
            translate([$center, $case_l*0.25, $wall]) rotate([0,90,0])
                cylinder(d=1.4*$wall, h=$case_l*0.5, center=true);
            translate([$case_l*0.75, $center, $wall]) rotate([90,0,0])
                cylinder(d=1.4*$wall, h=$case_l*0.5, center=true);
            translate([$case_l*0.25, $center, $wall]) rotate([90,0,0])
                cylinder(d=1.4*$wall, h=$case_l*0.5, center=true);
        };
        translate([$x_pause, $center, -1]) cylinder(d=$button_d, h=$wall+4);
        translate([$x_fr, $center + $x_d_fr, -1]) cylinder(d=$button_d, h=$wall+4);
        translate([$x_fr, $center - $x_d_fr, -1]) cylinder(d=$button_d, h=$wall+4);
        translate([$x_pause, $center, 1.5 * $wall + 1.2])
            cube([$button_d + 2*$wall, $gap, $wall+2], center=true);
        translate([$x_fr, $center + $x_d_fr, 1.5 * $wall + 1.2])
            cube([$button_d + 2*$wall, $gap, $wall+2], center=true);
        translate([$x_fr, $center - $x_d_fr, 1.5 * $wall + 1.2])
            cube([$button_d + 2*$wall, $gap, $wall+2], center=true);
        
        translate([$x_pause - $button_d * 0.9, $center, 0]) {
            translate([0, -$button_d * 0.1, 0]) cube([$button_d * 0.4, $button_d * 0.1, 1.5*$wall], center=true);
            translate([0, $button_d * 0.1, 0]) cube([$button_d * 0.4, $button_d * 0.1, 1.5*$wall], center=true);
        }
        translate([$x_fr - $button_d * 0.9, $center + $x_d_fr, 0]) fr(left=false);
        translate([$x_fr - $button_d * 0.9, $center - $x_d_fr, 0]) fr(left=true);
    };
};

case_buttons_volume();