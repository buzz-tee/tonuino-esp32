include <_dimensions.scad>;
include <_quality.scad>;

module case_skeleton() {
    module bar(edge) {
        $edge_x = (edge < 2) ? -$outer_width : $outer_width;
        $edge_y = (edge % 2 == 0) ? -$outer_width : $outer_width;
        translate([0,0,$inner/2]) {
            difference() {
                cube([$outer_width * 2, $outer_width * 2, $outer], center=true);
                translate([$edge_x, $edge_y, -0.5])
                    cube([$outer_width, $outer_width, $outer + 2], center=true);
                translate([$edge_x*1.5, 0, $outer/2+$outer_width/2]) rotate([90, 0, 0])
                    cube([$outer_width * 2, $outer_width * 2, $outer_width * 2 + 1], center=true);
                translate([0, $edge_y*1.5, $outer/2+$outer_width/2]) rotate([0, 90, 0])
                    cube([$outer_width * 2, $outer_width * 2, $outer_width * 2 + 1], center=true);
                translate([$edge_x*1.5, 0, -$outer/2-$outer_width/2]) rotate([90, 0, 0])
                    cube([$outer_width * 2, $outer_width * 2, $outer_width * 2 + 1], center=true);
                translate([0, $edge_y*1.5, -$outer/2-$outer_width/2]) rotate([0, 90, 0])
                    cube([$outer_width * 2, $outer_width * 2, $outer_width * 2 + 1], center=true);
            };
            translate([$edge_x/2, -$edge_y/4, $outer/2-$outer_width/2]) rotate([90, 0, 0])
                cylinder(d=$outer_width, h=$outer_width*1.5, center=true);
            translate([$edge_x/2, -$edge_y/4, -$outer/2+$outer_width/2]) rotate([90, 0, 0])
                cylinder(d=$outer_width, h=$outer_width*1.5, center=true);
            translate([-$edge_x/4, $edge_y/2, $outer/2-$outer_width/2]) rotate([0, 90, 0])
                cylinder(d=$outer_width, h=$outer_width*1.5, center=true);
            translate([-$edge_x/4, $edge_y/2, -$outer/2+$outer_width/2]) rotate([0, 90, 0])
                cylinder(d=$outer_width, h=$outer_width*1.5, center=true);
        }
        translate([$edge_x/2, $edge_y/2, -$outer_width/2])
            cylinder(d=$outer_width, h=$outer - $outer_width);
        translate([$edge_x/2, $edge_y/2, -$outer_width/2]) {
            sphere(d=$outer_width);
        }
        translate([$edge_x/2, $edge_y/2, $outer-1.5*$outer_width]) sphere(d=$outer_width);
    }
    
    $outer_width = 7.5;
    $inner = $case_l - 2 * $offset_frame;
    $outer = $inner + 2 * $outer_width;
    
    $tolerance = 0.1;
    
    difference() {
        union() {
            bar(0);
            rotate([-90,0,0]) bar(1);
            rotate([0,90,0]) bar(2);
            translate([$inner, 0, $inner]) {
                rotate([0,180,0]) bar(0);
                rotate([-90,0,0]) bar(2);
                rotate([0,-90,0]) bar(2);
            }
            translate([0, $inner, $inner]) {
                rotate([0,180,0]) bar(3);
                rotate([90,0,0]) bar(1);
                rotate([0,90,0]) bar(1);
            }
            translate([$inner, $inner, 0]) {
                rotate([0,0,0]) bar(3);
                rotate([90,0,0]) bar(2);
                rotate([0,-90,0]) bar(1);
            }
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
