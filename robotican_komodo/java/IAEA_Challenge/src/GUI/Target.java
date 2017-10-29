package GUI;

public class Target {
	
	private int x,y;
	private double angle, duration, lift;
	private boolean unknownTarget;
	
	public Target(int x,int y,double angle,double duration,double lift,boolean unknownTarget){
		this.x=x;
		this.y=y;
		this.angle=angle;
		this.duration=duration;
		this.lift=lift;
		this.unknownTarget=unknownTarget;
	}
	
	public int getX(){
		return x;
	}
	public int getY(){
		return y;
	}
	public double getAngle(){
		return angle;
	}
	public double getDuration(){
		return duration;
	}
	public double getLift(){
		return lift;
	}
	public boolean isUnknownTarget(){
		return unknownTarget;
	}
	public void setUnknownTarget(boolean unknownTarget){
		this.unknownTarget=unknownTarget;
	}
	public String toFile(double res, double imgHeight,double xOrigin,double yOrigin, double scale){
		return (x*res/scale+xOrigin) +" "+ (-y*res/scale+yOrigin+imgHeight*res/scale) +" "+angle+" "+duration+" "+lift+" "+unknownTarget;
	}
	
	public String toString(double res, double imgHeight,double xOrigin,double yOrigin, double scale){
		return "x: "+ (x*res/scale+xOrigin) +", y: "+ (-y*res/scale+yOrigin+imgHeight*res/scale) +", angle: "+angle+", duration: "+duration+", lift elevation: "+lift+" unknown target: "+unknownTarget;
	}

}