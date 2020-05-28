include <_dimensions.scad>;
include <_quality.scad>;

$f_in = 0.5;

module fr(left=true) {
    $x1 = -$button_d * 0.2;
    $x2 = $button_d * 0.2;
    $y1 = -$button_d * 0.12;
    $y2 = $button_d * 0.12;
    points = left ? [[$x1,$y2], [0,$y1], [$x2,$y2]] : [[$x1,$y1], [$x2,$y1], [0,$y2]];
    points2 = left ? [[$x1+$f_in,$y2-$f_in/2], [0,$y1+$f_in], [$x2-$f_in,$y2-$f_in/2]] :
                    [[$x1+$f_in,$y1+$f_in/2], [$x2-$f_in,$y1+$f_in/2], [0,$y2-$f_in]];
    
    translate([0,0,0.375*$wall]) {
        linear_extrude(height=0.75*$wall, center=true) {
            translate([0, -$button_d * 0.1, 0]) polygon(points);
            translate([0, $button_d * 0.1, 0]) polygon(points);
        };
    };
    linear_extrude(height=1.5*$wall, center=true) {
        translate([0, -$button_d * 0.1, 0]) polygon(points2);
        translate([0, $button_d * 0.1, 0]) polygon(points2);
    };
};

module button_pause() {
    translate([0,0,0.375*$wall]) {
        translate([0, -$button_d * 0.1, 0]) cube([$button_d * 0.4, $button_d * 0.1, 0.75*$wall], center=true);
        translate([0, $button_d * 0.1, 0]) cube([$button_d * 0.4, $button_d * 0.1, 0.75*$wall], center=true);
    };
    translate([0, -$button_d * 0.1, 0]) cube([$button_d * 0.4-$f_in, $button_d * 0.1-$f_in, 1.5*$wall], center=true);
    translate([0, $button_d * 0.1, 0]) cube([$button_d * 0.4-$f_in, $button_d * 0.1-$f_in, 1.5*$wall], center=true);
};

module button_plus() {
    translate([0,0,0.375*$wall]) {
        cube([$button_d * 0.5, $button_d * 0.1, 0.75*$wall], center=true);
        cube([$button_d * 0.1, $button_d * 0.5, 0.75*$wall], center=true);
    };
    cube([$button_d * 0.5-$f_in, $button_d * 0.1-$f_in, 1.5*$wall], center=true);
    cube([$button_d * 0.1-$f_in, $button_d * 0.5-$f_in, 1.5*$wall], center=true);
};

module button_minus() {
    translate([0,0,0.375*$wall])
        cube([$button_d * 0.1, $button_d * 0.5, 0.75*$wall], center=true);
    cube([$button_d * 0.1-$f_in, $button_d * 0.5-$f_in, 1.5*$wall], center=true);
};


button_plus();
translate([0, 15, 0]) button_minus();
translate([15, 0, 0]) fr(left=true);
translate([15, 15, 0]) fr(left=false);
translate([30, 0, 0]) button_pause();