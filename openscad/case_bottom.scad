include <case.scad>;
include <_quality.scad>;

module case_bottom() {
    $screw_mount_d=15;
    $screw_hole_d = 4.5; // actual 4
    $screw_mount_h = 7.5;
    $screw_head_d = 8.5;
    $screw_head_h = 5;
    $center = $case_l / 2;
    
    $docking_d = 54;
    $docking_h = 15;
    
    $power_d = 8;
    $power_d_max = 16;
    
    $screw_hole_diagon = sqrt(2 * pow($case_l - 2*$offset_frame - $screw_mount_d, 2));
    $nut_w = 7.1;
    $nut_l = 8;
    $nut_h = 3.2;
    
    difference() {
        union() {
            case(with_ears=false);
            translate([$offset_frame, $offset_frame, $wall/2]) cube([$screw_mount_d/2, $screw_mount_d/2, $wall*2.5]);
            translate([$case_l - $offset_frame - $screw_mount_d/2, $offset_frame, $wall/2]) cube([$screw_mount_d/2, $screw_mount_d/2, $wall*2.5]);
            translate([$offset_frame, $case_l - $offset_frame - $screw_mount_d/2, $wall/2]) cube([$screw_mount_d/2, $screw_mount_d/2, $wall*2.5]);
            translate([$case_l - $offset_frame - $screw_mount_d/2, $case_l - $offset_frame - $screw_mount_d/2, $wall/2]) cube([$screw_mount_d/2, $screw_mount_d/2, $wall*2.5]);
            
            translate([$offset_frame + $screw_mount_d/2, $offset_frame + $screw_mount_d/2, $wall/2]) {
                cylinder(d=$screw_mount_d, h=$screw_mount_h + $wall/2);
            }
            translate([$offset_frame + $screw_mount_d/2, $case_l - ($offset_frame + $screw_mount_d/2), $wall/2]) {
                cylinder(d=$screw_mount_d, h=$screw_mount_h + $wall/2);
            }
            translate([$case_l - ($offset_frame + $screw_mount_d/2), $offset_frame + $screw_mount_d/2, $wall/2]) {
                cylinder(d=$screw_mount_d, h=$screw_mount_h + $wall/2);
            }
            translate([$case_l - ($offset_frame + $screw_mount_d/2), $case_l - ($offset_frame + $screw_mount_d/2), $wall/2]) {
                cylinder(d=$screw_mount_d, h=$screw_mount_h + $wall/2);
            }
            translate([$center, $center, $wall/2]) {
                cylinder(d=$docking_d + 3*$wall, h=$docking_h + 2*$wall);
                cylinder(d=$power_d_max + 3*$wall, h=$docking_h + 3*$wall);
            };
        };
        translate([$offset_frame + $screw_mount_d/2, $offset_frame + $screw_mount_d/2, -$wall/2]) {
            cylinder(d=$screw_head_d, h=$screw_head_h + $wall/2);
            cylinder(d=$screw_hole_d, h=2 * $screw_mount_h + 6*$wall);
        };
        translate([$offset_frame + $screw_mount_d/2, $case_l - ($offset_frame + $screw_mount_d/2), -$wall/2]) {
            cylinder(d=$screw_head_d, h=$screw_head_h + $wall/2);
            cylinder(d=$screw_hole_d, h=2 * $screw_mount_h + 6*$wall);
        };
        translate([$case_l - ($offset_frame + $screw_mount_d/2), $offset_frame + $screw_mount_d/2, -$wall/2]) {
            cylinder(d=$screw_head_d, h=$screw_head_h + $wall/2);
            cylinder(d=$screw_hole_d, h=2 * $screw_mount_h + 6*$wall);
        };
        translate([$case_l - ($offset_frame + $screw_mount_d/2), $case_l - ($offset_frame + $screw_mount_d/2), -$wall/2]) {
            cylinder(d=$screw_head_d, h=$screw_head_h + $wall/2);
            cylinder(d=$screw_hole_d, h=2 * $screw_mount_h + 6*$wall);
        };
        translate([$center, $center, -$wall/2]) {
            cylinder(d=$docking_d, h=$docking_h + $wall/2);
            cylinder(d=$power_d_max, h=$docking_h + 2.5*$wall);
            cylinder(d=$power_d, h=$docking_h + 4.5*$wall);
        };
    };
};

case_bottom();