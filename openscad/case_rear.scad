include <case.scad>;
include <_quality.scad>;

module case_rear() {
    $battery_shield_w = 35;
    $battery_shield_l = 50;
    $battery_w = 45;
    $battery_l = 60;
    $battery_h = 13;

    case();

    $screw_dy = 23 * 2.54;
    $screw_dx = 27 * 2.54;
    $width = 67;
    $length = 77;
    $center = $case_l / 2;
    
    $screw_mount = 7.7;
    $mount_h = 2;

    difference() {
        union() {
            translate([$center, $center, $battery_h / 2 + 1.5 * $wall]) {
                cube([$length, $width, $battery_h + 1.5*$wall], center=true);
                for(x = [-1, 1]) for(y = [-1, 1]) translate([x * $screw_dx/2, y * $screw_dy/2, $battery_h / 2]) {
                    difference() {
                        cylinder(d=$screw_mount, h=$mount_h + $wall/2);
                        translate([0, 0, $wall/2 + $mount_h * 0.75]) cylinder(d=1, h=$mount_h);
                    };
                }
            };
            translate([$center - 5, $case_l - $offset_inner - 12, $wall / 2]) cube([10, 12, 2 + $wall * 1.5]);
        };
        translate([$center - $length / 2 - $wall, $center - $width / 2 + $wall, $wall])
            cube([$battery_l, $battery_w, $battery_h]);
        translate([$center - $length/2 - $wall, $case_l - $offset_inner - $battery_shield_w - $wall, $wall]) {
            cube([$battery_shield_l, $battery_shield_w - $wall - 5, $battery_h]);
            cube([$battery_shield_l + 2, $battery_shield_w -5, 2]);
        }
    };
    translate([$center - $length/2, $center + $width / 2 - 10, $wall/2])
        cube([10, 10, $battery_h + $wall]);
}

case_rear();