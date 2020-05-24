include <_dimensions.scad>;
include <_quality.scad>;

module nut_case() {
    $screw_mount_d=15;
    $screw_hole_d = 4.5; // actual 4
    $screw_mount_h = 7.5;
    $nut_w = 7.1;
    $nut_l = 8;
    $nut_h = 3.3;
    
    difference() {
        cylinder(d=$screw_mount_d, h=$nut_h+$wall);
        translate([0,0,-1]) cylinder(d=$screw_hole_d, $nut_h+2*$wall);
        translate([0,0, $nut_h+$wall]) cube([$nut_l, $nut_w, $nut_h*2], center=true);
    };
};

nut_case();
