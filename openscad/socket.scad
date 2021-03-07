include <_dimensions.scad>;
include <_quality.scad>;

$magnet_d = 10.2;
$magnet_h = 2.5;

$tag_d = 30;
$tag_h = 0.5 * $wall;
$floor_h = 0.5 * $wall;

$socket_h = $magnet_h + $wall + 5;
$socket_d = $tag_d + 3*$wall;

module socket() {
    
    difference() {
        cylinder(d=$socket_d, h=$socket_h);
        socket_floor();
        translate([0,0,-0.1]) {
            socket_floor();
            cylinder(d=$tag_d, h=$floor_h+$tag_h+0.1);
            cylinder(d=$magnet_d, h=$tag_h+$magnet_h+0.1);
        };
    };
};

module socket_floor(shrink_by=0) {
    intersection() {
        union() {
            cylinder(d=$tag_d+$wall-shrink_by, h=$floor_h);
            translate([0,0,0.5*$floor_h]) {
                cube([$socket_d+10, $socket_d/5-shrink_by, $floor_h], center=true);
                cube([$socket_d/5-shrink_by, $socket_d+10, $floor_h], center=true);
            };
        };
        cylinder(d=$socket_d+0.1-shrink_by, h=$floor_h+1);
    };
};


translate([0,0,$socket_h]) rotate([180,0,0])
    socket();

translate([$socket_d, 0, 0]) rotate([0,0,45])
    socket_floor(shrink_by=0.8);
