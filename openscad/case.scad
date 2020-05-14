$fn=20;

$wall = 1.4;
$case_l = 120;

module case(with_ears=true) {
    module screw_ear(l=15, h=15, left=true) {
        $screw_hole_d = 4;
        $offset_x = left ? (-2*$wall - 0.1) : 0.1;

        translate([$offset_x, -l/2, $wall/2]) {
            difference() {
                union() {
                    cube([$wall*2, l, h+$wall/2]);
                    translate([0, l/2, h+$wall/2]) rotate([0, 90, 0]) cylinder(d=l, h=$wall*2);
                };
                translate([-1, l/2, h+$wall/2]) rotate([0, 90, 0]) cylinder(d=$screw_hole_d, h=$wall*2 + 2);
            };
        };
    }
    
    intersection() {
        // cube([$case_l, $case_l, $wall]);
        translate([$case_l/2, -2*$case_l, 0]) cylinder(r=$case_l*3, h=$wall);
        translate([-2*$case_l, $case_l/2, 0]) cylinder(r=$case_l*3, h=$wall);
        translate([$case_l/2, $case_l*3, 0]) cylinder(r=$case_l*3, h=$wall);
        translate([$case_l*3, $case_l/2, 0]) cylinder(r=$case_l*3, h=$wall);
    };
    
    if (with_ears) {
        translate([8, 12.5, 0]) screw_ear(left=false);
        translate([$case_l - 8, 12.5, 0]) screw_ear(left=false);
        translate([8, $case_l - 12.5, 0]) screw_ear();
        translate([$case_l - 8, $case_l - 12.5, 0]) screw_ear();
    };
    
    difference() {
        translate([5, 5, $wall - 1]) cube([$case_l - 10, $case_l - 10, $wall * 2 + 1]);
        translate([5 + $wall, 5 + $wall, 0]) cube([$case_l - 10 - 2*$wall, $case_l - 10 - 2*$wall, $wall*3 + 1]);
    }
};
