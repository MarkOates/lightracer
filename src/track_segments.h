
/*
TrackSegment *create_track_segmentA()
{
	TrackSegment *segment = new TrackSegment();
	segment->add_point(LEFT_RAIL, 0, 400);
	segment->add_point(LEFT_RAIL, 0, 200);
	segment->add_point(LEFT_RAIL, 50, 100);
	segment->add_point(LEFT_RAIL, 50, 0);

	segment->add_point(RIGHT_RAIL, 150, 0);
	segment->add_point(RIGHT_RAIL, 150, 150);
	segment->add_point(RIGHT_RAIL, 100, 250);
	segment->add_point(RIGHT_RAIL, 100, 400);

	segment->move(0, -400);

	return segment;
}
*/

TrackSegment *create_track_segmentA()
{
	TrackSegment *segment = new TrackSegment();
	segment->add_point(LEFT_RAIL, 200, 500);
	segment->add_point(LEFT_RAIL, 200, 375);
	segment->add_point(LEFT_RAIL, 150, 275);
	segment->add_point(LEFT_RAIL, 150, 100);

	segment->add_point(RIGHT_RAIL, 250, 100);
	segment->add_point(RIGHT_RAIL, 250, 250);
	segment->add_point(RIGHT_RAIL, 300, 350);
	segment->add_point(RIGHT_RAIL, 300, 500);

	segment->move(-200, -500);

	return segment;
}


TrackSegment *create_track_segmentB()
{
	TrackSegment *segment = new TrackSegment();
	segment->add_point(LEFT_RAIL, 300, 500);
	segment->add_point(LEFT_RAIL, 300, 400);
	segment->add_point(LEFT_RAIL, 350, 350);
	segment->add_point(LEFT_RAIL, 350, 200);
	segment->add_point(LEFT_RAIL, 250, 100);
	segment->add_point(LEFT_RAIL, 0, 100);

	segment->add_point(RIGHT_RAIL, 0, 0);
	segment->add_point(RIGHT_RAIL, 450, 0);
	segment->add_point(RIGHT_RAIL, 450, 450);
	segment->add_point(RIGHT_RAIL, 400, 500);

	segment->move(-300, -500);

	return segment;
}


TrackSegment *create_track_segmentC()
{
	TrackSegment *segment = new TrackSegment();

	segment->add_point(LEFT_RAIL, 0, 500);
	segment->add_point(LEFT_RAIL, 0, 125);
	segment->add_point(LEFT_RAIL, 250, 50);

	segment->add_point(RIGHT_RAIL, 300, 500-363.396);
	segment->add_point(RIGHT_RAIL, 125, 225);
	segment->add_point(RIGHT_RAIL, 100, 500);

	segment->move(0, -500);

	return segment;
}


TrackSegment *create_track_segmentD()
{
	TrackSegment *segment = new TrackSegment();

	segment->add_point(LEFT_RAIL, 200, 500);
	segment->add_point(LEFT_RAIL, 200, 200);
	segment->add_point(LEFT_RAIL, 100, 100);
	segment->add_point(LEFT_RAIL, 0, 100);

	segment->add_point(RIGHT_RAIL, 0, 0);
	segment->add_point(RIGHT_RAIL, 150, 0);
	segment->add_point(RIGHT_RAIL, 300, 150);
	segment->add_point(RIGHT_RAIL, 300, 500);

	segment->move(-200, -500);

	return segment;
}



TrackSegment *create_track_segmentE()
{
	TrackSegment *segment = new TrackSegment();

	segment->add_point(LEFT_RAIL, 50, 1000);
	segment->add_point(LEFT_RAIL, 50, 650);
	segment->add_point(LEFT_RAIL, 0, 600);
	segment->add_point(LEFT_RAIL, 0, 200);
	segment->add_point(LEFT_RAIL, 50, 150);
	segment->add_point(LEFT_RAIL, 50, 0);

	segment->add_point(RIGHT_RAIL, 150, 0);
	segment->add_point(RIGHT_RAIL, 150, 150);
	segment->add_point(RIGHT_RAIL, 200, 200);
	segment->add_point(RIGHT_RAIL, 200, 600);
	segment->add_point(RIGHT_RAIL, 150, 650);
	segment->add_point(RIGHT_RAIL, 150, 1000);

	segment->move(-50, -1000);

	return segment;
}



TrackSegment *opening_gate()
{
	TrackSegment *segment = new TrackSegment();

	segment->add_point(LEFT_RAIL, 0, 500);
	segment->add_point(LEFT_RAIL, -50, 500);
	segment->add_point(LEFT_RAIL, -100, 425);
	segment->add_point(LEFT_RAIL, -100, 325);
	segment->add_point(LEFT_RAIL, -50, 225);
	segment->add_point(LEFT_RAIL, -50, 125);

	segment->add_point(RIGHT_RAIL, 50, 125);
	segment->add_point(RIGHT_RAIL, 50, 225);
	segment->add_point(RIGHT_RAIL, 100, 325);
	segment->add_point(RIGHT_RAIL, 100, 425);
	segment->add_point(RIGHT_RAIL, 50, 500);
	segment->add_point(RIGHT_RAIL, 0, 500);

	segment->move(50, 475);

	return segment;
}