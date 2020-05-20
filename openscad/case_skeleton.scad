include <_dimensions.scad>;
include <_quality.scad>;

module case_skeleton() {
    $outer_width = 7.5;
    $outer = $case_l - 2 * $offset_frame + 2 * $outer_width;
    $inner = $case_l - 2 * $offset_frame;
    
    $tolerance = 0.1;
    
    difference() {
        union() {
            cylinder(r=$outer_width, h=$inner);
            rotate([-90,0,0]) cylinder(r=$outer_width, h=$inner);
            rotate([0,90,0]) cylinder(r=$outer_width, h=$inner);
            sphere(r=$outer_width);
            translate([$inner, 0, $inner]) {
                rotate([0,180,0]) cylinder(r=$outer_width, h=$inner);
                rotate([-90,0,0]) cylinder(r=$outer_width, h=$inner);
                rotate([0,-90,0]) cylinder(r=$outer_width, h=$inner);
                sphere(r=$outer_width);
            }
            translate([0, $inner, $inner]) {
                rotate([0,180,0]) cylinder(r=$outer_width, h=$inner);
                rotate([90,0,0]) cylinder(r=$outer_width, h=$inner);
                rotate([0,90,0]) cylinder(r=$outer_width, h=$inner);
                sphere(r=$outer_width);
            }
            translate([$inner, $inner, 0]) {
                rotate([0,0,0]) cylinder(r=$outer_width, h=$inner);
                rotate([90,0,0]) cylinder(r=$outer_width, h=$inner);
                rotate([0,-90,0]) cylinder(r=$outer_width, h=$inner);
                sphere(r=$outer_width);
            }
            translate([0, 0, $inner]) sphere(r=$outer_width);
            translate([0, $inner, 0]) sphere(r=$outer_width);
            translate([$inner, 0, 0]) sphere(r=$outer_width);
            translate([$inner, $inner, $inner]) sphere(r=$outer_width);
        };

        translate([-$outer_width - 1, -$tolerance, -$tolerance])
            cube([$outer + 2, $inner + 2 * $tolerance, $inner + 2 * $tolerance]);
        translate([-$tolerance, -$outer_width - 1, -$tolerance])
            cube([$inner + 2 * $tolerance, $outer + 2, $inner + 2 * $tolerance]);
        translate([-$tolerance, -$tolerance, -$outer_width - 1])
            cube([$inner + 2 * $tolerance, $inner + 2 * $tolerance, $outer + 2]);
    };
};

case_skeleton();