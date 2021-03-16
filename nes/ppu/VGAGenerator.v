// VGAGenerator
//
// - Generate 'x','y' pixel position signals to drive video renderer
// - Generate 'visible' signal, showing video renderer when it is valid to drive external VGA r,g,b signals
// - Parameters default to Visible 640x480 (including non-visible: 800x525)

module VGAGenerator
#(
    parameter WIDTH = 800,   
    parameter HEIGHT = 525,
    parameter WIDTH_VISIBLE = 640,
    parameter HEIGHT_VISIBLE = 480,

    parameter BIT_DEPTH = 11
)
(
    input i_clk,
    input i_reset_n,

    output [BIT_DEPTH-1:0] o_x,
    output [BIT_DEPTH-1:0] o_y,
    output o_visible
);
    reg [BIT_DEPTH-1:0] r_x;
    reg [BIT_DEPTH-1:0] r_y;
    
    always @(negedge i_reset_n or posedge i_clk)
        begin
            if (!i_reset_n)
            begin
                r_x <= 0;
                r_y <= 0;
            end
            else
            begin
                if (r_x == (WIDTH-1))
                begin
                    r_x <= 0;
                    
                    if (r_y == (HEIGHT-1))
                    begin
                        r_y <= 0;
                    end           
                    else
                    begin
                        r_y <= r_y + 1;
                    end
                end
                else
                begin
                    r_x <= r_x + 1;
                end              
            end
        end
    
    assign o_x = r_x;
    assign o_y = r_y;
    assign o_visible = (r_x < WIDTH_VISIBLE) && (r_y < HEIGHT_VISIBLE);

endmodule