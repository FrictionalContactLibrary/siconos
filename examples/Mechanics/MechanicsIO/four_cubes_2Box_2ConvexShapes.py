#!/usr/bin/env python

#
# Example of two cubes, one with a convex shape, one with a primitive
# shape.
#

from siconos.mechanics.collision.tools import Contactor

from siconos.io.mechanics_io import Hdf5

import siconos.numerics as Numerics


edge_length = 0.1
plane_length = 2.0

velocity_init=0.0
angular_velocity_init =0.0

# Creation of the hdf5 file for input/output
with Hdf5() as io:

    # # Definition of a cube as a convex shape
    io.addConvexShape('CubeCS1', [
        (-edge_length, edge_length, -edge_length),
        (-edge_length, -edge_length, -edge_length),
        (-edge_length, -edge_length, edge_length),
        (-edge_length, edge_length, edge_length),
        (edge_length, edge_length, edge_length),
        (edge_length, edge_length, -edge_length),
        (edge_length, -edge_length, -edge_length),
        (edge_length, -edge_length, edge_length)])

    io.addConvexShape('CubeCS2', [
        (-edge_length, edge_length, -edge_length),
        (-edge_length, -edge_length, -edge_length),
        (-edge_length, -edge_length, edge_length),
        (-edge_length, edge_length, edge_length),
        (edge_length, edge_length, edge_length),
        (edge_length, edge_length, -edge_length),
        (edge_length, -edge_length, -edge_length),
        (edge_length, -edge_length, edge_length)])

    # Alternative to the previous convex shape definition.
    io.addPrimitiveShape('CubePrim1', 'Box', (2*edge_length, 2*edge_length, 2*edge_length))

    io.addPrimitiveShape('CubePrim2', 'Box', (2*edge_length, 2*edge_length, 2*edge_length))

    # Definition of the ground shape
    io.addPrimitiveShape('Ground', 'Box', (plane_length, plane_length, plane_length/10.0))

    # Definition of a non smooth law. As no group ids are specified it
    # is between contactors of group id 0.
    io.addNewtonImpactFrictionNSL('contact', mu=0.3,e=0.5)

    # The cube object made with an unique Contactor : the cube shape.
    # As a mass is given, it is a dynamic system involved in contact
    # detection and in the simulation.  With no group id specified the
    # Contactor belongs to group 0
    io.addObject('cube1', [Contactor('CubeCS1')], translation=[0, 0, 2],
                 velocity=[velocity_init, 0, 0,  angular_velocity_init, angular_velocity_init, angular_velocity_init],
                 mass=1)

    io.addObject('cube2', [Contactor('CubeCS2')], translation=[0, 0, 2+3*edge_length],
                 velocity=[velocity_init, 0, 0, angular_velocity_init, angular_velocity_init, angular_velocity_init],
                 mass=1)

    io.addObject('cubeP1', [Contactor('CubePrim1')], translation=[0, 3*edge_length, 2],
                 velocity=[velocity_init, 0, 0, angular_velocity_init, angular_velocity_init, angular_velocity_init],
                 mass=1)

    io.addObject('cubeP2', [Contactor('CubePrim2')], translation=[0, 3*edge_length, 2+3*edge_length],
                 velocity=[velocity_init, 0, 0, angular_velocity_init, angular_velocity_init, angular_velocity_init],
                 mass=1)

    # the ground object made with the ground shape. As the mass is
    # not given, it is a static object only involved in contact
    # detection.
    io.addObject('ground', [Contactor('Ground')],
                 translation=[0, 0, 0])


# Run the simulation from the inputs previously defined and add
# results to the hdf5 file. The visualisation of the output may be done
# with the vview command.


step=1000
hstep=0.005

with Hdf5(mode='r+',collision_margin=0.05) as io:

    # By default earth gravity is applied and the units are those
    # of the International System of Units.
    # Because of fixed collision margins used in the collision detection,
    # sizes of small objects may need to be expressed in cm or mm.

    io.run(with_timer=False,
           time_stepping=None,
           space_filter=None,
           body_class=None,
           shape_class=None,
           face_class=None,
           edge_class=None,
           gravity_scale=0.1,
           t0=0,
           T=step*hstep,
           h=hstep,
           multipoints_iterations=True,
           theta=0.50001,
           Newton_max_iter=1,
           set_external_forces=None,
           solver=Numerics.SICONOS_FRICTION_3D_NSGS,
           itermax=100,
           tolerance=1e-4,
           numerics_verbose=False,
           output_frequency=10,
           violation_verbose=True)
