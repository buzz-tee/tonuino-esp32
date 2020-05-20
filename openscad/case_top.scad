include <case.scad>;
include <_quality.scad>;

module case_top() {
    $screw_mount_d=15;
    $screw_hole_d = 4.5; // actual 4
    $screw_mount_h = 7.5;
    $center = $case_l / 2;
    
    $screw_hole_diagon = sqrt(2 * pow($case_l - 2*$offset_frame - $screw_mount_d, 2));
    $nut_w = 7.1;
    $nut_l = 8;
    $nut_h = 3.2;
    
    $rfid_hole = 4.9;
    $rfid_hole_d = 2.75; 
    $rfid_mount_h = 4;  // ~2 magnets
    $rfid_mount_d = $rfid_hole + 2*$wall;
    $rfid_dy1 = 12.5;
    $rfid_dy2 = 17;
    $rfid_dx1 = 18.5;
    $rfid_dx2 = 23;
    $magnet_d = 10;

    difference() {
        union() {
            case(with_ears=false);
            
            translate([$offset_frame, $offset_frame, $wall/2]) cube([$screw_mount_d/2, $screw_mount_d/2, $wall*2.5]);
            translate([$case_l - $offset_frame - $screw_mount_d/2, $offset_frame, $wall/2]) cube([$screw_mount_d/2, $screw_mount_d/2, $wall*2.5]);
            translate([$offset_frame, $case_l - $offset_frame - $screw_mount_d/2, $wall/2]) cube([$screw_mount_d/2, $screw_mount_d/2, $wall*2.5]);
            translate([$case_l - $offset_frame - $screw_mount_d/2, $case_l - $offset_frame - $screw_mount_d/2, $wall/2]) cube([$screw_mount_d/2, $screw_mount_d/2, $wall*2.5]);
            
            translate([$offset_frame + $screw_mount_d/2, $offset_frame + $screw_mount_d/2, $wall/2])
                cylinder(d=$screw_mount_d, h=$screw_mount_h + $wall/2);
            translate([$offset_frame + $screw_mount_d/2, $case_l - ($offset_frame + $screw_mount_d/2), $wall/2])
                cylinder(d=$screw_mount_d, h=$screw_mount_h + $wall/2);
            translate([$case_l - ($offset_frame + $screw_mount_d/2), $offset_frame + $screw_mount_d/2, $wall/2])
                cylinder(d=$screw_mount_d, h=$screw_mount_h + $wall/2);
            translate([$case_l - ($offset_frame + $screw_mount_d/2), $case_l - ($offset_frame + $screw_mount_d/2), $wall/2])
                cylinder(d=$screw_mount_d, h=$screw_mount_h + $wall/2);
        };
        translate([$offset_frame + $screw_mount_d/2, $offset_frame + $screw_mount_d/2, 1.5 * $wall])
            cylinder(d=$screw_hole_d, h=$screw_mount_h);
        translate([$offset_frame + $screw_mount_d/2, $case_l - ($offset_frame + $screw_mount_d/2), 1.5 * $wall])
            cylinder(d=$screw_hole_d, h=$screw_mount_h);
        translate([$case_l - ($offset_frame + $screw_mount_d/2), $offset_frame + $screw_mount_d/2, 1.5 * $wall])
            cylinder(d=$screw_hole_d, h=$screw_mount_h);
        translate([$case_l - ($offset_frame + $screw_mount_d/2), $case_l - ($offset_frame + $screw_mount_d/2), 1.5 * $wall])
            cylinder(d=$screw_hole_d, h=$screw_mount_h);
        
        translate([$center, $center, $wall * 1.5 + $nut_h / 2]) rotate([0,0,45]) cube([$screw_hole_diagon + $nut_l, $nut_w, $nut_h], center=true);
        translate([$center, $center, $wall * 1.5 + $nut_h / 2]) rotate([0,0,-45]) cube([$screw_hole_diagon + $nut_l, $nut_w, $nut_h], center=true);
    }

    difference() {
        union() {
            translate([$center + $rfid_dx1, $center + $rfid_dy1, $wall/2])
                cylinder(d=$rfid_mount_d, h=$rfid_mount_h + $wall/2);
            translate([$center + $rfid_dx1, $center - $rfid_dy1, $wall/2])
                cylinder(d=$rfid_mount_d, h=$rfid_mount_h + $wall/2);
            translate([$center - $rfid_dx2, $center + $rfid_dy2, $wall/2])
                cylinder(d=$rfid_mount_d, h=$rfid_mount_h + $wall/2);
            translate([$center - $rfid_dx2, $center - $rfid_dy2, $wall/2])
                cylinder(d=$rfid_mount_d, h=$rfid_mount_h + $wall/2);
            translate([$center, $center, $wall/2])
                cylinder(d=$magnet_d + 4*$wall, h=$rfid_mount_h + $wall/2);
            translate([$center, $center, 1.25*$wall]) {
                cube([$case_l - 2*$offset_inner, $wall, 1.5*$wall], center=true);
                cube([$wall, $case_l - 2*$offset_inner, 1.5*$wall], center=true);
            };
        };
        translate([$center + $rfid_dx1, $center + $rfid_dy1, $wall])
            cylinder(d=$rfid_hole_d, h=$rfid_mount_h + $wall/2);
        translate([$center + $rfid_dx1, $center - $rfid_dy1, $wall])
            cylinder(d=$rfid_hole_d, h=$rfid_mount_h + $wall/2);
        translate([$center - $rfid_dx2, $center + $rfid_dy2, $wall])
            cylinder(d=$rfid_hole_d, h=$rfid_mount_h + $wall/2);
        translate([$center - $rfid_dx2, $center - $rfid_dy2, $wall])
            cylinder(d=$rfid_hole_d, h=$rfid_mount_h + $wall/2);
        translate([$center, $center, $wall])
            cylinder(d=$magnet_d, h=$rfid_mount_h + $wall/2);
    };
    
    difference() {
        translate([$center, $center, $wall/2]) {
            cylinder(d=$case_l*0.8, h=1.5*$wall);
        };
        translate([$center, $center, $wall]) {
            cylinder(d=$case_l*0.8 - 2*$wall, h=1.5*$wall);
        };
    };
}

case_top();
