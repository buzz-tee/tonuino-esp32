include <case.scad>;
include <quality.scad>;

module case_speaker() {
    module spk_cage_ring(od, w, h=$wall) {
        difference() {
            cylinder(d=od / 100 * $spk_clear_d, h=h);
            if (w > 0) translate([0, 0, -1]) cylinder(d=(od - w) / 100 * $spk_clear_d, h=h+2);
        }
    };
    $spk_d = 100;
    $spk_clear_d = 95;
    $screw_dist = 85;
    
    $center = $case_l/2;
    $screw_offset = $screw_dist/2;
    $screw_hole = 4.9;
    $screw_hole_d = 2.5;
    $screw_mount = $screw_hole + 2*$wall;
    
    $mount_h = 3.75;
    
    difference() {
        union() {
            case();
            for(x = [-1, 1]) for(y = [-1, 1])
                translate([$center+$screw_offset*x, $center+$screw_offset*y, $wall])
                    cylinder(d=$screw_mount, h=$mount_h);
        };
        translate([$center, $center, -1]) cylinder(d=$spk_clear_d, h=$wall+2);
        for(x = [-1, 1]) for(y = [-1, 1])
            translate([$center+$screw_offset*x, $center+$screw_offset*y, $wall/2])
                cylinder(d=$screw_hole_d, h=$mount_h+$wall);
    };
    translate([$center, $center, 0]) {
        spk_cage_ring(78, 6);
        spk_cage_ring(48, 6);
        spk_cage_ring(18, 0);
        
        translate([0, 0, $wall/2]) {
            rotate([0, 0, 45]) cube([$spk_d + 5, 8, $wall], center=true);
            rotate([0, 0, -45]) cube([$spk_d + 5, 8, $wall], center=true);
            
            difference() {
                cylinder(d=$spk_d + 2*$wall, h=$wall*2.5);
                translate([0, 0, -1]) cylinder(d=$spk_d, h=$wall*2.5 + 2);
            };
        }
    };
};

case_speaker();
