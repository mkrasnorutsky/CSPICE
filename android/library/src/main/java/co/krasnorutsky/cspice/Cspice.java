package co.krasnorutsky.cspice;

/**
 * Java wrapper for the shared CSPICE asteroid ephemeris API ({@code Asteroids.h}).
 *
 * <p>Call {@link #init(String)} once with the app documents directory before any other method.
 * SPK/BSP kernel files are loaded from that directory on first use of the native singleton.
 */
public final class Cspice {
    static {
        System.loadLibrary("cspice");
    }

    private Cspice() {
    }

    /** Stores the documents path used to load {@code Asteroids.json} and SPK files. */
    public static native void init(String docsPath);

    /** Loads additional ephemeris files from {@code path}. */
    public static native boolean loadEpheFiles(String path);

    public static native boolean objectIsPresent(int naifId);

    public static native boolean jdIsPresent(int naifId, double jd);

    /** Returns the asteroid name for a NAIF ID, or an empty string. */
    public static native String asteroidName(int naifId);

    public static native int lastObserverId();

    public static native int[] loadedSpkIds();

    public static native int[] recentlyAddedIds();

    /**
     * Computes heliocentric state for {@code jplId} at Julian date {@code jd}.
     *
     * @param obsId observer NAIF ID, or {@code 0} to infer from SPK metadata
     */
    public static native AsteroidResult calculate(double jd, int jplId, int obsId);

    public static final class AsteroidResult {
        public final double posX;
        public final double posY;
        public final double posZ;
        public final double velX;
        public final double velY;
        public final double velZ;
        public final boolean valid;
        public final String error;

        public AsteroidResult(
                double posX,
                double posY,
                double posZ,
                double velX,
                double velY,
                double velZ,
                boolean valid,
                String error) {
            this.posX = posX;
            this.posY = posY;
            this.posZ = posZ;
            this.velX = velX;
            this.velY = velY;
            this.velZ = velZ;
            this.valid = valid;
            this.error = error != null ? error : "";
        }
    }
}
